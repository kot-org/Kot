#include <kot/heap.h>

struct Heap globalHeap;
static uint64_t mutexHeap;

void InitializeHeap(){
    SYS_GetProcessKey(&globalHeap.process);
    globalHeap.heapEnd = &globalHeap;
    SYS_Map(globalHeap.process, &globalHeap.heapEnd, false, 0, 0x1000, true);

    struct SegmentHeader* newSegment = (struct SegmentHeader*)globalHeap.heapEnd;

    globalHeap.heapEnd += 0x1000;
    newSegment->singature = 0xff;
    newSegment->length = 0x1000 - sizeof(struct SegmentHeader);
    newSegment->IsFree = true;
    newSegment->last = NULL;
    newSegment->next = NULL;
    globalHeap.lastSegment = newSegment;    
    globalHeap.mainSegment = newSegment; 
    
    globalHeap.TotalSize += 0x1000 + sizeof(struct SegmentHeader);     
    globalHeap.FreeSize += 0x1000 + sizeof(struct SegmentHeader); 
    atomicUnlock(&mutexHeap, 0);   
}

void* calloc(size_t size){
    void* address = malloc(size);
    memset(address, 0, size);
    return address;
}

void* malloc(size_t size){
    if (size == 0) return NULL;
    
    if(size % 0x10 > 0){ // it is not a multiple of 0x10
        size -= (size % 0x10);
        size += 0x10;
    }

    atomicSpinlock(&mutexHeap, 0);
    atomicLock(&mutexHeap, 0);

    struct SegmentHeader* currentSeg = (struct SegmentHeader*)globalHeap.mainSegment;
    while(true){
        if(currentSeg->IsFree){
            if(currentSeg->length > size){
                // split this segment in two 
                SplitSegment(currentSeg, size);
                currentSeg->IsFree = false;
                globalHeap.UsedSize += currentSeg->length + sizeof(struct SegmentHeader);
                globalHeap.FreeSize -= currentSeg->length + sizeof(struct SegmentHeader);
                atomicUnlock(&mutexHeap, 0);
                return (void*)((uint64_t)currentSeg + sizeof(struct SegmentHeader));
            }else if(currentSeg->length == size){
                currentSeg->IsFree = false;
                globalHeap.UsedSize += currentSeg->length + sizeof(struct SegmentHeader);
                globalHeap.FreeSize -= currentSeg->length + sizeof(struct SegmentHeader);
                atomicUnlock(&mutexHeap, 0);
                return (void*)((uint64_t)currentSeg + sizeof(struct SegmentHeader));
            }
        }
        if (currentSeg->next == NULL) break;
        currentSeg = currentSeg->next;
    }
    
    ExpandHeap(size);
    atomicUnlock(&mutexHeap, 0);
    return malloc(size);
}


void MergeNextAndThisToLast(struct SegmentHeader* header){
    // merge this segment into the last segment
    if(header->next == globalHeap.lastSegment){
        if(header->next->next != NULL){
            globalHeap.lastSegment = header->next->next;
        }else{
            globalHeap.lastSegment = header->last;
        }
    }
    if(header->next == globalHeap.mainSegment){
        if(header->next->last != NULL){
            globalHeap.mainSegment = header->next->last;
        }else{
            globalHeap.mainSegment = header->next->next;
        }
    }
    if(header == globalHeap.lastSegment){
        if(header->next->next != NULL){
            globalHeap.lastSegment = header->next->next;
        }else{
            globalHeap.lastSegment = header->last;
        }
    }
    if(header == globalHeap.mainSegment){
        if(header->last != NULL){
            globalHeap.mainSegment = header->last;
        }else{
            globalHeap.mainSegment = header->next->next;
        }
    }
    
    header->last->length += header->length + sizeof(struct SegmentHeader) + header->next->length + sizeof(struct SegmentHeader);
    header->last->next = header->next->next;
    header->next->next->last = header->last;
    memset(header->next, 0, sizeof(struct SegmentHeader));
    memset(header, 0, sizeof(struct SegmentHeader));
}

void MergeThisToLast(struct SegmentHeader* header){
    // merge this segment into the last segment
    header->last->length += header->length + sizeof(struct SegmentHeader);
    header->last->next = header->next;
    header->next->last = header->last;
    if(header == globalHeap.lastSegment){
        if(header->next != NULL){
            globalHeap.lastSegment = header->next;
        }else{
            globalHeap.lastSegment = header->last;
        }
    }
    if(header == globalHeap.mainSegment){
        if(header->last != NULL){
            globalHeap.mainSegment = header->last;
        }else{
            globalHeap.mainSegment = header->next;
        }
    }
    memset(header, 0, sizeof(struct SegmentHeader));
}

void MergeNextToThis(struct SegmentHeader* header){
    // merge this segment into the next segment

    struct SegmentHeader* headerNext = header->next;
    header->length += header->next->length + sizeof(struct SegmentHeader);
    header->next = header->next->next;
    if(header->next != NULL) header->next->last = header;
    if(headerNext == globalHeap.lastSegment){
        globalHeap.lastSegment = header;
    }

    memset(headerNext, 0, sizeof(struct SegmentHeader));
}

void free(void* address){
    if(address != NULL){
        atomicSpinlock(&mutexHeap, 0);
        atomicLock(&mutexHeap, 0);
        struct SegmentHeader* header = (struct SegmentHeader*)(void*)((uint64_t)address - sizeof(struct SegmentHeader));
        header->IsFree = true;
        globalHeap.FreeSize += header->length + sizeof(struct SegmentHeader);
        globalHeap.UsedSize -= header->length + sizeof(struct SegmentHeader);

        if(header->next != NULL  && header->last != NULL){
            if(header->next->IsFree && header->last->IsFree){
                // merge this segment and next segment into the last segment
                MergeNextAndThisToLast(header);
                atomicUnlock(&mutexHeap, 0);
                return;
            }
        }
        if(header->last != NULL){
            if(header->last->IsFree){
                // merge this segment into the last segment
                MergeThisToLast(header);  
                atomicUnlock(&mutexHeap, 0); 
                return;  
            }         
        }
        
        if(header->next != NULL){
            if(header->next->IsFree){
                // merge this segment into the next segment
                MergeNextToThis(header);
                atomicUnlock(&mutexHeap, 0);
                return; 
            }
        }

        atomicUnlock(&mutexHeap, 0);
    }
}

void* realloc(void* buffer, size_t size){
    void* newBuffer = malloc(size);

    if(size < GetSegmentHeader(buffer)->length){
        memcpy(newBuffer, buffer, size);
    }else{
        memcpy(newBuffer, buffer, GetSegmentHeader(buffer)->length);
    }

    free(buffer);
    return newBuffer;
}

void SplitSegment(struct SegmentHeader* segment, size_t size){
    if(segment->length > size + sizeof(struct SegmentHeader)){
        struct SegmentHeader* newSegment = (struct SegmentHeader*)(void*)((uint64_t)segment + sizeof(struct SegmentHeader) + (uint64_t)size);
        memset(newSegment, 0, sizeof(struct SegmentHeader));
        newSegment->IsFree = true;         
        newSegment->singature = 0xff;       
        newSegment->length = segment->length - (size + sizeof(struct SegmentHeader));
        newSegment->next = segment->next;
        newSegment->last = segment;

        if(segment->next == NULL){
            globalHeap.lastSegment = newSegment;
        }
        segment->next = newSegment;
        segment->length = size;        
    }

}

void ExpandHeap(size_t length){
    length += sizeof(struct SegmentHeader);

    SYS_Map(globalHeap.process, &globalHeap.heapEnd, false, 0, length, false);

    struct SegmentHeader* newSegment = (struct SegmentHeader*)globalHeap.heapEnd;
    globalHeap.heapEnd += length;

    if(globalHeap.lastSegment != NULL && globalHeap.lastSegment->IsFree){
        globalHeap.lastSegment->length += length;
    }else{
        newSegment->singature = 0xff;
        newSegment->length = length - sizeof(struct SegmentHeader);
        newSegment->IsFree = true;
        newSegment->last = globalHeap.lastSegment;
        newSegment->next = NULL;
        if(globalHeap.lastSegment != NULL){
            globalHeap.lastSegment->next = newSegment;
        }
        globalHeap.lastSegment = newSegment;        
    }

    if(globalHeap.mainSegment == NULL){
        globalHeap.mainSegment = newSegment;
    }   
    
    globalHeap.TotalSize += length + sizeof(struct SegmentHeader);     
    globalHeap.FreeSize += length + sizeof(struct SegmentHeader);     
}

struct SegmentHeader* GetSegmentHeader(void* address){
    return (struct SegmentHeader*)(void*)((uint64_t)address - sizeof(struct SegmentHeader));
}
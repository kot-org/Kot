#include <kot/heap.h>

static uint64_t mutexHeap;
static struct heap_t heap;

void InitializeHeapUser(){
    heap.Process = Sys_GetProcess();
    heap.EndAddress = KotSpecificData.HeapLocation;
    ExpandHeapUser(0x1000);
    atomicUnlock(&mutexHeap, 0);   
    heap.IsHeapEnabled = true;
}

uintptr_t calloc(size64_t size){
    uintptr_t address = malloc(size);
    memset(address, 0, size);
    return address;
}

uintptr_t malloc(size64_t size){
    if(!heap.IsHeapEnabled){
        InitializeHeapUser();
    }

    if(size == 0) return NULL;
    
    if(size % 0x10 > 0){ // it is not a multiple of 0x10
        size -= (size % 0x10);
        size += 0x10;
    }

    atomicAcquire(&mutexHeap, 0);

    struct SegmentHeader* currentSeg = (struct SegmentHeader*)heap.mainSegment;
    while(true){
        if(currentSeg->IsFree){
            if(currentSeg->length > size){
                // split this segment in two 
                SplitSegmentUser(currentSeg, size);
                currentSeg->IsFree = false;
                heap.UsedSize += currentSeg->length + sizeof(struct SegmentHeader);
                heap.FreeSize -= currentSeg->length + sizeof(struct SegmentHeader);
                atomicUnlock(&mutexHeap, 0);
                return (uintptr_t)((uint64_t)currentSeg + sizeof(struct SegmentHeader));
            }else if(currentSeg->length == size){
                currentSeg->IsFree = false;
                heap.UsedSize += currentSeg->length + sizeof(struct SegmentHeader);
                heap.FreeSize -= currentSeg->length + sizeof(struct SegmentHeader);
                atomicUnlock(&mutexHeap, 0);
                return (uintptr_t)((uint64_t)currentSeg + sizeof(struct SegmentHeader));
            }
        }
        if (currentSeg->next == NULL) break;
        currentSeg = currentSeg->next;
    }
    
    ExpandHeapUser(size);
    atomicUnlock(&mutexHeap, 0);
    return malloc(size);
}


void MergeNextAndThisToLastUser(struct SegmentHeader* header){
    MergeNextToThisUser(header);
    MergeThisToLastUser(header);
}

void MergeThisToLastUser(struct SegmentHeader* header){
    // merge this segment into the last segment
    header->last->length += header->length + sizeof(struct SegmentHeader);
    header->last->next = header->next;
    if(header->next != NULL){
        header->next->last = header->last;
    }
    if(header == heap.lastSegment){
        if(header->next != NULL){
            heap.lastSegment = header->next;
        }else{
            heap.lastSegment = header->last;
        }
    }
    if(header == heap.mainSegment){
        if(header->last != NULL){
            heap.mainSegment = header->last;
        }else{
            heap.mainSegment = header->next;
        }
    }
    memset(header, 0, sizeof(struct SegmentHeader));
}

void MergeNextToThisUser(struct SegmentHeader* header){
    // merge this segment into the next segment

    struct SegmentHeader* headerNext = header->next;
    header->length += header->next->length + sizeof(struct SegmentHeader);
    header->next = header->next->next;
    if(header->next != NULL) header->next->last = header;
    if(headerNext == heap.lastSegment){
        heap.lastSegment = header;
    }

    memset(headerNext, 0, sizeof(struct SegmentHeader));
}

void free(uintptr_t address){
    if(address != NULL){
        atomicAcquire(&mutexHeap, 0);
        struct SegmentHeader* header = (struct SegmentHeader*)(uintptr_t)((uint64_t)address - sizeof(struct SegmentHeader));
        header->IsFree = true;
        heap.FreeSize += header->length + sizeof(struct SegmentHeader);
        heap.UsedSize -= header->length + sizeof(struct SegmentHeader);

        if(header->next != NULL  && header->last != NULL){
            if(header->next->IsFree && header->last->IsFree){
                // merge this segment and next segment into the last segment
                MergeNextAndThisToLastUser(header);
                atomicUnlock(&mutexHeap, 0);
                return;
            }
        }
        if(header->last != NULL){
            if(header->last->IsFree){
                // merge this segment into the last segment
                MergeThisToLastUser(header);  
                atomicUnlock(&mutexHeap, 0); 
                return;  
            }         
        }
        
        if(header->next != NULL){
            if(header->next->IsFree){
                // merge this segment into the next segment
                MergeNextToThisUser(header);
                atomicUnlock(&mutexHeap, 0);
                return; 
            }
        }

        atomicUnlock(&mutexHeap, 0);
    }
}

uintptr_t realloc(uintptr_t buffer, size64_t size){
    uintptr_t newBuffer = malloc(size);

    if(size < GetSegmentHeaderUser(buffer)->length){
        memcpy(newBuffer, buffer, size);
    }else{
        memcpy(newBuffer, buffer, GetSegmentHeaderUser(buffer)->length);
    }

    free(buffer);
    return newBuffer;
}

void SplitSegmentUser(struct SegmentHeader* segment, size64_t size){
    if(segment->length > size + sizeof(struct SegmentHeader)){
        struct SegmentHeader* newSegment = (struct SegmentHeader*)(uintptr_t)((uint64_t)segment + sizeof(struct SegmentHeader) + (uint64_t)size);
        newSegment->IsFree = true;   
        newSegment->signature = 0xff;       
        newSegment->length = segment->length - (size + sizeof(struct SegmentHeader));
        newSegment->next = segment->next;
        newSegment->last = segment;

        if(segment->next == NULL){
            heap.lastSegment = newSegment;
        }
        segment->next = newSegment;
        segment->length = size;        
    }
}

void ExpandHeapUser(size64_t length){
    length += sizeof(struct SegmentHeader);
    
    Sys_Map(heap.Process, &heap.EndAddress, AllocationTypeBasic, 0, &length, false);

    struct SegmentHeader* newSegment = (struct SegmentHeader*)heap.EndAddress;
    heap.EndAddress += length;

    if(heap.lastSegment != NULL && heap.lastSegment->IsFree){
        heap.lastSegment->length += length;
    }else{
        newSegment->signature = 0xff;
        newSegment->length = length - sizeof(struct SegmentHeader);
        newSegment->IsFree = true;
        newSegment->last = heap.lastSegment;
        newSegment->next = NULL;
        if(heap.lastSegment != NULL){
            heap.lastSegment->next = newSegment;
        }
        heap.lastSegment = newSegment;
    }

    if(heap.mainSegment == NULL){
        heap.mainSegment = newSegment;
    }   
    
    heap.TotalSize += length;     
    heap.FreeSize += length;     
}

struct SegmentHeader* GetSegmentHeaderUser(uintptr_t address){
    return (struct SegmentHeader*)(uintptr_t)((uint64_t)address - sizeof(struct SegmentHeader));
}
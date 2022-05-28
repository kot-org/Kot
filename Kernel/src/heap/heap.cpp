#include <heap/heap.h>
#include <arch/arch.h>

Heap globalHeap;

void InitializeHeap(uintptr_t heapAddress, size_t pageCount){
    globalHeap.heapEnd = heapAddress;
    uintptr_t NewPhysicalAddress = Pmm_RequestPage();
    globalHeap.heapEnd = (uintptr_t)((uint64_t)globalHeap.heapEnd - PAGE_SIZE);
    vmm_Map(vmm_PageTable, globalHeap.heapEnd, NewPhysicalAddress, false, true, true);
    globalHeap.mainSegment = (SegmentHeader*)((uint64_t)globalHeap.heapEnd + (PAGE_SIZE - sizeof(SegmentHeader)));
    globalHeap.mainSegment->singature = 0xff;
    globalHeap.mainSegment->length = 0;
    globalHeap.mainSegment->IsFree = false;
    globalHeap.mainSegment->last = NULL;
    globalHeap.mainSegment->next = (SegmentHeader*)((uint64_t)globalHeap.mainSegment - PAGE_SIZE + sizeof(SegmentHeader));

    globalHeap.mainSegment->next->IsFree = true;
    globalHeap.mainSegment->next->length = PAGE_SIZE - sizeof(SegmentHeader) - sizeof(SegmentHeader); /* remove twice because we have the main and new header in the same page */
    globalHeap.mainSegment->next->singature = 0xff;
    globalHeap.mainSegment->next->last = globalHeap.mainSegment;
    globalHeap.mainSegment->next->next = NULL;
    globalHeap.lastSegment = globalHeap.mainSegment->next;  

    globalHeap.TotalSize += PAGE_SIZE;     
    globalHeap.FreeSize += PAGE_SIZE;  
}

uintptr_t calloc(size_t size){
    uintptr_t address = malloc(size);
    memset(address, 0, size);
    return address;
}

uintptr_t malloc(size_t size){
    if (size == 0) return NULL;

    if(size % 0x10 > 0){ // it is not a multiple of 0x10
        size -= (size % 0x10);
        size += 0x10;
    }

    Atomic::atomicAcquire(&globalHeap.lock, 0);

    SegmentHeader* currentSeg = (SegmentHeader*)globalHeap.mainSegment;
    uint64_t SizeWithHeader = size + sizeof(SegmentHeader);
    while(true){
        if(currentSeg->IsFree){
            if(currentSeg->length > SizeWithHeader){
                // split this segment in two 
                currentSeg = SplitSegment(currentSeg, size);
                currentSeg->IsFree = false;
                globalHeap.UsedSize += currentSeg->length + sizeof(SegmentHeader);
                globalHeap.FreeSize -= currentSeg->length + sizeof(SegmentHeader);
                Atomic::atomicUnlock(&globalHeap.lock, 0);
                return (uintptr_t)((uint64_t)currentSeg + sizeof(SegmentHeader));
            }else if(currentSeg->length == size){
                currentSeg->IsFree = false;
                globalHeap.UsedSize += currentSeg->length + sizeof(SegmentHeader);
                globalHeap.FreeSize -= currentSeg->length + sizeof(SegmentHeader);
                Atomic::atomicUnlock(&globalHeap.lock, 0);
                return (uintptr_t)((uint64_t)currentSeg + sizeof(SegmentHeader));
            }
        }
        if (currentSeg->next == NULL) break;
        currentSeg = currentSeg->next;
    }
    
    ExpandHeap(size);
    Atomic::atomicUnlock(&globalHeap.lock, 0);
    return malloc(size);
}


void MergeNextAndThisToLast(SegmentHeader* header){
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
    
    header->last->length += header->length + sizeof(SegmentHeader) + header->next->length + sizeof(SegmentHeader);
    header->last->next = header->next->next;
    header->next->next->last = header->last;
    memset(header->next, 0, sizeof(SegmentHeader));
    memset(header, 0, sizeof(SegmentHeader));
}

void MergeThisToLast(SegmentHeader* header){
    // merge this segment into the last segment
    header->last->length += header->length + sizeof(SegmentHeader);
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
    memset(header, 0, sizeof(SegmentHeader));
}

void MergeNextToThis(SegmentHeader* header){
    // merge this segment into the next segment

    SegmentHeader* headerNext = header->next;
    header->length += header->next->length + sizeof(SegmentHeader);
    header->next = header->next->next;
    if(header->next != NULL) header->next->last = header;
    if(headerNext == globalHeap.lastSegment){
        globalHeap.lastSegment = header;
    }

    memset(headerNext, 0, sizeof(SegmentHeader));
}

void free(uintptr_t address){
    if(address != NULL){
        Atomic::atomicAcquire(&globalHeap.lock, 0);
        
        SegmentHeader* header = (SegmentHeader*)(uintptr_t)((uint64_t)address - sizeof(SegmentHeader));
        header->IsFree = true;
        globalHeap.FreeSize += header->length + sizeof(SegmentHeader);
        globalHeap.UsedSize -= header->length + sizeof(SegmentHeader);

        if(header->next != NULL  && header->last != NULL){
            if(header->next->IsFree && header->last->IsFree){
                // merge this segment and next segment into the last segment
                MergeNextAndThisToLast(header);
                Atomic::atomicUnlock(&globalHeap.lock, 0);
                return;
            }
        }
        if(header->last != NULL){
            if(header->last->IsFree){
                // merge this segment into the last segment
                MergeThisToLast(header);  
                Atomic::atomicUnlock(&globalHeap.lock, 0); 
                return;  
            }         
        }
        
        if(header->next != NULL){
            if(header->next->IsFree){
                // merge this segment into the next segment
                MergeNextToThis(header);
                Atomic::atomicUnlock(&globalHeap.lock, 0);
                return; 
            }
        }

        Atomic::atomicUnlock(&globalHeap.lock, 0);
    }
}

uintptr_t realloc(uintptr_t buffer, size_t size){
    uintptr_t newBuffer = malloc(size);

    if(size < GetSegmentHeader(buffer)->length){
        memcpy(newBuffer, buffer, size);
    }else{
        memcpy(newBuffer, buffer, GetSegmentHeader(buffer)->length);
    }

    free(buffer);
    return newBuffer;
}

SegmentHeader* SplitSegment(SegmentHeader* segment, size_t size){
    if(segment->length > size + sizeof(SegmentHeader)){
        SegmentHeader* newSegment = (SegmentHeader*)(uintptr_t)((uint64_t)segment + segment->length - size);
        memset(newSegment, 0, sizeof(SegmentHeader));
        newSegment->IsFree = true;         
        newSegment->singature = 0xff;       
        newSegment->length = size;
        newSegment->next = segment;
        newSegment->last = segment->last;

        if(segment->next == NULL){
            globalHeap.lastSegment = segment;
        }

        if(segment->last != NULL){
            segment->last->next = newSegment;
        }
        segment->last = newSegment;
        segment->length = segment->length - (size + sizeof(SegmentHeader));  
        return newSegment;      
    }
    return NULL;
}

void ExpandHeap(size_t length){
    length += sizeof(SegmentHeader);
    if(length % PAGE_SIZE){
        length -= length % PAGE_SIZE;
        length += PAGE_SIZE;
    }

    size_t pageCount = length / PAGE_SIZE;


    for (size_t i = 0; i < pageCount; i++){
        uintptr_t NewPhysicalAddress = Pmm_RequestPage();
        globalHeap.heapEnd = (uintptr_t)((uint64_t)globalHeap.heapEnd - PAGE_SIZE);
        vmm_Map(vmm_PageTable, globalHeap.heapEnd, NewPhysicalAddress, false, true, true);
    }

    SegmentHeader* newSegment = (SegmentHeader*)globalHeap.heapEnd;

    if(globalHeap.lastSegment != NULL && globalHeap.lastSegment->IsFree && globalHeap.lastSegment->last != NULL){
        uint64_t size = globalHeap.lastSegment->length + length;
        newSegment->singature = 0xff;
        newSegment->length = size - sizeof(SegmentHeader);
        newSegment->IsFree = true;
        newSegment->last = globalHeap.lastSegment->last;
        newSegment->last->next = newSegment;
        newSegment->next = NULL;
        globalHeap.lastSegment = newSegment;    
    }else{
        newSegment->singature = 0xff;
        newSegment->length = length - sizeof(SegmentHeader);
        newSegment->IsFree = true;
        newSegment->last = globalHeap.lastSegment;
        newSegment->next = NULL;
        if(globalHeap.lastSegment != NULL){
            globalHeap.lastSegment->next = newSegment;
        }
        globalHeap.lastSegment = newSegment;        
    }  
    
    globalHeap.TotalSize += length + sizeof(SegmentHeader);     
    globalHeap.FreeSize += length + sizeof(SegmentHeader);     
}

SegmentHeader* GetSegmentHeader(uintptr_t address){
    return (SegmentHeader*)(uintptr_t)((uint64_t)address - sizeof(SegmentHeader));
}
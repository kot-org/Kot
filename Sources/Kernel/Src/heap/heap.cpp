#include <heap/heap.h>
#include <arch/arch.h>

Heap globalHeap;

void InitializeHeap(uintptr_t heapAddress, size64_t pageCount){
    globalHeap.heapEnd = heapAddress;
    uintptr_t NewPhysicalAddress = Pmm_RequestPage();
    globalHeap.heapEnd = (uintptr_t)((uint64_t)globalHeap.heapEnd - PAGE_SIZE);
    vmm_Map(vmm_PageTable, globalHeap.heapEnd, NewPhysicalAddress, false, true, true);
    globalHeap.mainSegment = (SegmentHeader*)((uint64_t)globalHeap.heapEnd + ((uint64_t)PAGE_SIZE - sizeof(SegmentHeader)));
    globalHeap.mainSegment->signature = 0xff;
    globalHeap.mainSegment->length = 0;
    globalHeap.mainSegment->IsFree = false;
    globalHeap.mainSegment->last = NULL;
    globalHeap.mainSegment->next = (SegmentHeader*)((uint64_t)globalHeap.mainSegment - (uint64_t)PAGE_SIZE + sizeof(SegmentHeader));

    globalHeap.mainSegment->next->IsFree = true;
    globalHeap.mainSegment->next->length = (uint64_t)PAGE_SIZE - sizeof(SegmentHeader) - sizeof(SegmentHeader); /* remove twice because we have the main and new header in the same page */
    globalHeap.mainSegment->next->signature = 0xff;
    globalHeap.mainSegment->next->last = globalHeap.mainSegment;
    globalHeap.mainSegment->next->next = NULL;
    globalHeap.lastSegment = globalHeap.mainSegment->next;  

    globalHeap.TotalSize += PAGE_SIZE;     
    globalHeap.FreeSize += PAGE_SIZE;  
}

uintptr_t calloc(size64_t size){
    uintptr_t address = malloc(size);
    memset(address, 0, size);
    return address;
}

uintptr_t calloc_WL(size64_t size){
    uintptr_t address = malloc_WL(size);
    memset(address, 0, size);
    return address;
}

uintptr_t malloc(size64_t size){
    Aquire(&globalHeap.lock);
    uintptr_t address = malloc_WL(size);
    Release(&globalHeap.lock);
    return address;
}

uintptr_t malloc_WL(size64_t size){
    if (size == 0) return NULL;

    if(size % 0x10 > 0){ // it is not a multiple of 0x10
        size -= (size % 0x10);
        size += 0x10;
    }

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
                return (uintptr_t)((uint64_t)currentSeg + sizeof(SegmentHeader));
            }else if(currentSeg->length == size){
                currentSeg->IsFree = false;
                globalHeap.UsedSize += currentSeg->length + sizeof(SegmentHeader);
                globalHeap.FreeSize -= currentSeg->length + sizeof(SegmentHeader);
                return (uintptr_t)((uint64_t)currentSeg + sizeof(SegmentHeader));
            }
        }
        if(currentSeg->next == NULL) break;
        currentSeg = currentSeg->next;
    }
    
    ExpandHeap(size);
    return malloc_WL(size);
}

void MergeThisToNext(SegmentHeader* header){
    // merge this segment into the last segment
    SegmentHeader* headerNext = header->next;
    headerNext->length += header->length + sizeof(SegmentHeader);
    headerNext->last = header->last;
    header->last->next = headerNext;

    memset(header, 0, sizeof(SegmentHeader));
}

void MergeLastToThis(SegmentHeader* header){
    // merge this segment into the next segment

    SegmentHeader* headerLast = header->last;
    header->length += headerLast->length + sizeof(SegmentHeader);
    header->last = headerLast->last;
    header->last->next = header;

    memset(headerLast, 0, sizeof(SegmentHeader));
}

void MergeLastAndThisToNext(SegmentHeader* header){
    // merge this segment into the last segment
    MergeLastToThis(header);
    MergeThisToNext(header);
}

void free(uintptr_t address){
        Aquire(&globalHeap.lock);
        free_WL(address);
        Release(&globalHeap.lock);
}

void free_WL(uintptr_t address){
    if(address != NULL){        
        SegmentHeader* header = (SegmentHeader*)(uintptr_t)((uint64_t)address - sizeof(SegmentHeader));
        header->IsFree = true;
        globalHeap.FreeSize += header->length + sizeof(SegmentHeader);
        globalHeap.UsedSize -= header->length + sizeof(SegmentHeader);

        if(header->next != NULL  && header->last != NULL){
            if(header->next->IsFree && header->last->IsFree){
                // merge this segment and next segment into the last segment
                MergeLastAndThisToNext(header);
                return;
            }
        }

        if(header->last != NULL){
            if(header->last->IsFree){
                // merge this segment into the last segment
                MergeLastToThis(header);  
                return;  
            }         
        }
        
        if(header->next != NULL){
            if(header->next->IsFree){
                // merge this segment into the next segment
                MergeThisToNext(header);
                return; 
            }
        }
    }
}

uintptr_t realloc(uintptr_t buffer, size64_t size){
    uintptr_t newBuffer = malloc(size);

    if(buffer != NULL){
        if(size < GetSegmentHeader(buffer)->length){
            memcpy(newBuffer, buffer, size);
        }else{
            memcpy(newBuffer, buffer, GetSegmentHeader(buffer)->length);
        }

        free(buffer);        
    }

    return newBuffer;
}

uintptr_t realloc_WL(uintptr_t buffer, size64_t size){
    uintptr_t newBuffer = malloc_WL(size);

    if(buffer != NULL){
        if(size < GetSegmentHeader(buffer)->length){
            memcpy(newBuffer, buffer, size);
        }else{
            memcpy(newBuffer, buffer, GetSegmentHeader(buffer)->length);
        }

        free_WL(buffer);        
    }

    return newBuffer;
}

SegmentHeader* SplitSegment(SegmentHeader* segment, size64_t size){
    if(segment->length > size + sizeof(SegmentHeader)){
        SegmentHeader* newSegment = (SegmentHeader*)(uintptr_t)((uint64_t)segment + segment->length - size);
        memset(newSegment, 0, sizeof(SegmentHeader));
        newSegment->IsFree = true;         
        newSegment->signature = 0xff;       
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

void ExpandHeap(size64_t length){
    length += sizeof(SegmentHeader);
    if(length % PAGE_SIZE){
        length -= length % PAGE_SIZE;
        length += PAGE_SIZE;
    }

    size64_t pageCount = length / PAGE_SIZE;


    for (size64_t i = 0; i < pageCount; i++){
        uintptr_t NewPhysicalAddress = Pmm_RequestPage();
        globalHeap.heapEnd = (uintptr_t)((uint64_t)globalHeap.heapEnd - (uint64_t)PAGE_SIZE);
        vmm_Map(vmm_PageTable, globalHeap.heapEnd, NewPhysicalAddress, false, true, true);
    }

    SegmentHeader* newSegment = (SegmentHeader*)globalHeap.heapEnd;

    if(globalHeap.lastSegment != NULL && globalHeap.lastSegment->IsFree && globalHeap.lastSegment->last != NULL){
        uint64_t size = globalHeap.lastSegment->length + length;
        newSegment->signature = 0xff;
        newSegment->length = size - sizeof(SegmentHeader);
        newSegment->IsFree = true;
        newSegment->last = globalHeap.lastSegment->last;
        newSegment->last->next = newSegment;
        newSegment->next = NULL;
        globalHeap.lastSegment = newSegment;    
    }else{
        newSegment->signature = 0xff;
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

void AcquireHeap(){
    Aquire(&globalHeap.lock);
}

void ReleaseHeap(){
    Release(&globalHeap.lock);
}
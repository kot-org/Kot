#include "heap.h"
#include "../paging/PageTableManager.h"
#include "../paging/PageFrameAllocator.h"

Heap globalHeap;

//Heap globalHeap;

void InitializeHeap(void* heapAddress, size_t pageCount){
    globalHeap.heapEnd = heapAddress;
    ExpandHeap(pageCount * 0x1000);
}

void* malloc(size_t size){
    if(size % 0x10 > 0){ // it is not a multiple of 0x10
        size -= (size % 0x10);
        size += 0x10;
    }

    if (size == 0) return NULL;

    SegmentHeader* currentSeg = (SegmentHeader*)globalHeap.mainSegment;
    while(true){
        if(currentSeg->IsFree){
            if(currentSeg->length > size){
                // split this segment in two 
                SegmentHeader* newSegment = (SegmentHeader*)(void*)((uint64_t)currentSeg + size);
                newSegment->IsFree = true;
                newSegment->length = currentSeg->length - (size + sizeof(SegmentHeader));
                newSegment->next = currentSeg->next;
                newSegment->last = currentSeg;
                currentSeg->next = newSegment;
                currentSeg->length = size;

                currentSeg->IsFree = false;
                globalHeap.UsedSize += currentSeg->length;
                globalHeap.FreeSize -= currentSeg->length;
                return (void*)((uint64_t)currentSeg + sizeof(SegmentHeader));
            }else if(currentSeg->length == size){
                currentSeg->IsFree = false;
                globalHeap.UsedSize += currentSeg->length;
                globalHeap.FreeSize -= currentSeg->length;
                return (void*)((uint64_t)currentSeg + sizeof(SegmentHeader));
            }
        }
        if (currentSeg->next == NULL) break;
        currentSeg = currentSeg->next;
    }
    ExpandHeap(size);
    return malloc(size);
}

void* realloc(void* buffer, size_t size, uint64_t adjustement){
    void* newBuffer = malloc(size);

    if(adjustement >= 0){
        memcpy(newBuffer, (void*)((uint64_t)buffer + adjustement), size - adjustement);
    }else{
        memcpy(newBuffer, buffer, size - adjustement);
    }
    
    free(buffer);
    return 0;
}

void free(void* address){
    if(address != NULL){
        SegmentHeader* header = (SegmentHeader*)((uint64_t)address - sizeof(SegmentHeader));
        header->IsFree = true;
        globalHeap.FreeSize += header->length;
        globalHeap.UsedSize -= header->length;

        if(header->last->IsFree && !header->next->IsFree){
            // merge this segment into the last segment
            header->last->next = header->next;
            header->next->last = header->last;
            header->last->length += header->length;
            memset(header, 0, sizeof(SegmentHeader));
        }else if(header->next->IsFree && !header->last->IsFree){
            // merge this segment into the next segment
            header->next->last = header->last;
            header->last->next = header->next;
            header->next->length = header->length;
            memset(header, 0, sizeof(SegmentHeader));
        }else if(header->next->IsFree && header->last->IsFree){
            // merge this segment and next segment into the last segment
            header->last->next = header->next->next;
            header->next->next->last = header->last;
            header->last->length += header->length + header->next->length;
            memset(header->next, 0, sizeof(SegmentHeader));
            memset(header, 0, sizeof(SegmentHeader));
        }
    }
}

void ExpandHeap(size_t length){
    length += sizeof(SegmentHeader);
    if(length % 0x1000){
        length -= length % 0x1000;
        length += 0x1000;
    }

    size_t pageCount = length / 0x1000;

    SegmentHeader* newSegment = (SegmentHeader*)globalHeap.heapEnd;

    for (size_t i = 0; i < pageCount; i++){
        globalPageTableManager.MapMemory(globalHeap.heapEnd, globalAllocator.RequestPage());
        globalHeap.heapEnd += 0x1000;
    }
    newSegment->length = length - sizeof(SegmentHeader);
    newSegment->IsFree = true;
    newSegment->IsUser = false;
    newSegment->last = globalHeap.lastSegment;
    if(globalHeap.lastSegment != NULL){
        globalHeap.lastSegment->next = newSegment;
    }
    globalHeap.lastSegment = newSegment;

    if(globalHeap.mainSegment == NULL){
        globalHeap.mainSegment = newSegment;
    }   

    globalHeap.TotalSize += length;     
    globalHeap.FreeSize += length;     
}


#include "heap.h"
#include "../paging/PageTableManager.h"
#include "../paging/PageFrameAllocator.h"

Heap globalHeap;

void volatile InitializeHeap(void* heapAddress, size_t pageCount){
    globalHeap.heapEnd = heapAddress;
    ExpandHeap(pageCount * 0x1000);
}

void volatile SegmentTracker(){
    SegmentHeader* currentSeg = (SegmentHeader*)globalHeap.mainSegment;
    globalCOM1->Print("Heap segments : \n");
    while(true){
        if(currentSeg->IsFree){
            globalCOM1->Print(SerialGREEN); globalCOM1->Write((char)220); globalCOM1->Print(SerialReset);
        }else{
            globalCOM1->Print(SerialRED); globalCOM1->Write((char)220); globalCOM1->Print(SerialReset);
        }

        if(currentSeg->next == NULL) break;
        currentSeg = currentSeg->next;
    }
    globalCOM1->Print("\n");
}

void* volatile malloc(size_t size){
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
                SplitSegment(currentSeg, size);
                currentSeg->IsFree = false;
                globalHeap.UsedSize += currentSeg->length + sizeof(SegmentHeader);
                globalHeap.FreeSize -= currentSeg->length + sizeof(SegmentHeader);
                return (void*)((uint64_t)currentSeg + sizeof(SegmentHeader));
            }else if(currentSeg->length == size){
                currentSeg->IsFree = false;
                globalHeap.UsedSize += currentSeg->length + sizeof(SegmentHeader);
                globalHeap.FreeSize -= currentSeg->length + sizeof(SegmentHeader);
                return (void*)((uint64_t)currentSeg + sizeof(SegmentHeader));
            }

        }
        if (currentSeg->next == NULL) break;
        currentSeg = currentSeg->next;
    }
    ExpandHeap(size);
    return malloc(size);
}

void volatile free(void* address){
    if(address != NULL){
        SegmentHeader* header = (SegmentHeader*)(void*)((uint64_t)address - sizeof(SegmentHeader));
        header->IsFree = true;
        globalHeap.FreeSize += header->length + sizeof(SegmentHeader);
        globalHeap.UsedSize -= header->length + sizeof(SegmentHeader);

        if(header->last->IsFree && !header->next->IsFree && header->last != 0){
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
        }else if(header->next->IsFree && !header->last->IsFree && header->next != 0){
            // merge this segment into the next segment
            SegmentHeader* headerNext = header->next;
            header->length += header->next->length + sizeof(SegmentHeader);
            header->next = header->next->next;
            header->next->last = header;
            if(header == globalHeap.lastSegment){
                if(header->next->next != NULL){
                    globalHeap.lastSegment = header->next->next;
                }else{
                    globalHeap.lastSegment = header;
                }
            }
            if(header == globalHeap.mainSegment){
                globalHeap.mainSegment = header->last;
            }

            memset(headerNext, 0, sizeof(SegmentHeader));
        }else if(header->next->IsFree && header->last->IsFree && header->next != 0  && header->last != 0){
            // merge this segment and next segment into the last segment
            header->last->length += header->length + sizeof(SegmentHeader) + header->next->length + sizeof(SegmentHeader);
            header->last->next = header->next->next;
            header->next->next->last = header->last;
            if(header->next == globalHeap.lastSegment){
                if(header->next->next != NULL){
                    globalHeap.lastSegment = header->next->next;
                }else{
                    globalHeap.lastSegment = header->next->last;
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
            memset(header->next, 0, sizeof(SegmentHeader));
            memset(header, 0, sizeof(SegmentHeader));
        }
    }
}

void* volatile realloc(void* buffer, size_t size, uint64_t adjustement){
    void* newBuffer = malloc(size);

    if(adjustement >= 0){
        memcpy(newBuffer, (void*)((uint64_t)buffer + adjustement), size - adjustement);
    }else{
        memcpy(newBuffer, buffer, size - adjustement);
    }
    
    free(buffer);
    return newBuffer;
}

void volatile SplitSegment(SegmentHeader* segment, size_t size){
    if(segment->length > size + sizeof(SegmentHeader)){
        SegmentHeader* newSegment = (SegmentHeader*)(void*)((uint64_t)segment + sizeof(SegmentHeader) + (uint64_t)size);
        memset(newSegment, 0, sizeof(SegmentHeader));
        newSegment->IsFree = true;         
        newSegment->length = segment->length - (size + sizeof(SegmentHeader));
        newSegment->next = segment->next;
        newSegment->last = segment;

        if(segment->next == NULL){
            globalHeap.lastSegment = newSegment;
        }
        segment->next = newSegment;
        segment->length = size;        
    }

}

void volatile ExpandHeap(size_t length){
    length += sizeof(SegmentHeader);
    if(length % 0x1000){
        length -= length % 0x1000;
        length += 0x1000;
    }

    size_t pageCount = length / 0x1000;

    SegmentHeader* newSegment = (SegmentHeader*)globalHeap.heapEnd;

    for (size_t i = 0; i < pageCount; i++){
        void* NewPhysicalAddress = globalAllocator.RequestPage();
        globalPageTableManager.MapMemory(globalHeap.heapEnd, NewPhysicalAddress);
        globalHeap.heapEnd = (void*)((uint64_t)globalHeap.heapEnd + 0x1000);
    }

    if(globalHeap.lastSegment != NULL && globalHeap.lastSegment->IsFree){
        globalHeap.lastSegment->length += length;
    }else{
        newSegment->length = length - sizeof(SegmentHeader);
        newSegment->IsFree = true;
        newSegment->IsUser = false;
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
    
    globalHeap.TotalSize += length + sizeof(SegmentHeader);     
    globalHeap.FreeSize += length + sizeof(SegmentHeader);     
}
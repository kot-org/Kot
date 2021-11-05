#include "heap.h"

namespace UserHeap{
    Heap* InitializeHeap(void* heapAddress, size_t pageCount, PageTableManager* UserPageTable){
        Heap* heap = (Heap*)calloc(sizeof(Heap));
        heap->heapEnd = heapAddress;
        heap->UserPageTable = UserPageTable;
        heap->ExpandHeap(pageCount * 0x1000);
        return heap;
    }
    
    void* Heap::calloc(size_t size){
        void* address = malloc(size);
        memset(address, 0, size);
        return address;
    }

    void* Heap::malloc(size_t size){
        if(size % 0x10 > 0){ // it is not a multiple of 0x10
            size -= (size % 0x10);
            size += 0x10;
        }

        if (size == 0) return NULL;

        SegmentHeader* currentSeg = (SegmentHeader*)this->mainSegment;
        while(true){
            if(currentSeg->IsFree){
                if(currentSeg->length > size){
                    // split this segment in two 
                    SplitSegment(currentSeg, size);
                    currentSeg->IsFree = false;
                    this->UsedSize += currentSeg->length + sizeof(SegmentHeader);
                    this->FreeSize -= currentSeg->length + sizeof(SegmentHeader);
                    return (void*)((uint64_t)currentSeg + sizeof(SegmentHeader));
                }else if(currentSeg->length == size){
                    currentSeg->IsFree = false;
                    this->UsedSize += currentSeg->length + sizeof(SegmentHeader);
                    this->FreeSize -= currentSeg->length + sizeof(SegmentHeader);
                    return (void*)((uint64_t)currentSeg + sizeof(SegmentHeader));
                }
            }
            if (currentSeg->next == NULL) break;
            currentSeg = currentSeg->next;
        }
        ExpandHeap(size);
        return malloc(size);
    }


    void Heap::MergeNextAndThisToLast(SegmentHeader* header){
        // merge this segment into the last segment
        if(header->next == this->lastSegment){
            if(header->next->next != NULL){
                this->lastSegment = header->next->next;
            }else{
                this->lastSegment = header->last;
            }
        }
        if(header->next == this->mainSegment){
            if(header->next->last != NULL){
                this->mainSegment = header->next->last;
            }else{
                this->mainSegment = header->next->next;
            }
        }
        if(header == this->lastSegment){
            if(header->next->next != NULL){
                this->lastSegment = header->next->next;
            }else{
                this->lastSegment = header->last;
            }
        }
        if(header == this->mainSegment){
            if(header->last != NULL){
                this->mainSegment = header->last;
            }else{
                this->mainSegment = header->next->next;
            }
        }
        
        header->last->length += header->length + sizeof(SegmentHeader) + header->next->length + sizeof(SegmentHeader);
        header->last->next = header->next->next;
        header->next->next->last = header->last;
        memset(header->next, 0, sizeof(SegmentHeader));
        memset(header, 0, sizeof(SegmentHeader));
    }

    void Heap::MergeThisToLast(SegmentHeader* header){
        // merge this segment into the last segment
        header->last->length += header->length + sizeof(SegmentHeader);
        header->last->next = header->next;
        header->next->last = header->last;
        if(header == this->lastSegment){
            if(header->next != NULL){
                this->lastSegment = header->next;
            }else{
                this->lastSegment = header->last;
            }
        }
        if(header == this->mainSegment){
            if(header->last != NULL){
                this->mainSegment = header->last;
            }else{
                this->mainSegment = header->next;
            }
        }
        memset(header, 0, sizeof(SegmentHeader));
    }

    void Heap::MergeNextToThis(SegmentHeader* header){
        // merge this segment into the next segment
        SegmentHeader* headerNext = header->next;
        header->length += header->next->length + sizeof(SegmentHeader);
        header->next = header->next->next;
        if(header->next != NULL){
            header->next->last = header;
        }
        
        if(header == this->lastSegment){
            if(header->next->next != NULL){
                this->lastSegment = header->next->next;
            }else{
                this->lastSegment = header;
            }
        }
        if(headerNext == this->lastSegment){
            if(header->next != NULL){
                this->lastSegment = header->next;
            }else{
                this->lastSegment = header;
            }
        }

        if(header == this->mainSegment){
            this->mainSegment = header->last;
        }

        memset(headerNext, 0, sizeof(SegmentHeader));
    }

    void Heap::free(void* address){
        if(address != NULL){
            SegmentHeader* header = (SegmentHeader*)(void*)((uint64_t)address - sizeof(SegmentHeader));
            header->IsFree = true;
            this->FreeSize += header->length + sizeof(SegmentHeader);
            this->UsedSize -= header->length + sizeof(SegmentHeader);

            if(header->next != NULL  && header->last != NULL){
                if(header->next->IsFree && header->last->IsFree){
                    // merge this segment and next segment into the last segment
                    MergeNextAndThisToLast(header);
                }
            }else if(header->last != NULL){
                if(header->last->IsFree){ 
                    // merge this segment into the last segment
                    MergeThisToLast(header);              
                }
            }else if(header->next != NULL){
                if(header->next->IsFree){ 
                    // merge this segment into the next segment
                    MergeNextToThis(header);
                }
            }
        }
    }

    void* Heap::realloc(void* buffer, size_t size, uint64_t adjustement){
        void* newBuffer = malloc(size);

        if(adjustement >= 0){
            memcpy(newBuffer, (void*)((uint64_t)buffer + adjustement), size - adjustement);
        }else{
            memcpy(newBuffer, buffer, size - adjustement);
        }
        
        free(buffer);
        return newBuffer;
    }

    void Heap::SplitSegment(SegmentHeader* segment, size_t size){
        if(segment->length > size + sizeof(SegmentHeader)){
            SegmentHeader* newSegment = (SegmentHeader*)(void*)((uint64_t)segment + sizeof(SegmentHeader) + (uint64_t)size);
            memset(newSegment, 0, sizeof(SegmentHeader));
            newSegment->IsFree = true;         
            newSegment->length = segment->length - (size + sizeof(SegmentHeader));
            newSegment->next = segment->next;
            newSegment->last = segment;

            if(segment->next == NULL){
                this->lastSegment = newSegment;
            }
            segment->next = newSegment;
            segment->length = size;        
        }

    }

    void Heap::ExpandHeap(size_t length){
        length += sizeof(SegmentHeader);
        if(length % 0x1000){
            length -= length % 0x1000;
            length += 0x1000;
        }

        size_t pageCount = length / 0x1000;

        SegmentHeader* newSegment = (SegmentHeader*)this->heapEnd;

        for (size_t i = 0; i < pageCount; i++){
            void* NewPhysicalAddress = globalAllocator.RequestPage();
            UserPageTable->MapMemory(this->heapEnd, NewPhysicalAddress);
            UserPageTable->MapUserspaceMemory(this->heapEnd);
            this->heapEnd = (void*)((uint64_t)this->heapEnd + 0x1000);
        }

        if(this->lastSegment != NULL && this->lastSegment->IsFree){
            this->lastSegment->length += length;
        }else{
            newSegment->length = length - sizeof(SegmentHeader);
            newSegment->IsFree = true;
            newSegment->last = this->lastSegment;
            newSegment->next = NULL;
            if(this->lastSegment != NULL){
                this->lastSegment->next = newSegment;
            }
            this->lastSegment = newSegment;        
        }

        if(this->mainSegment == NULL){
            this->mainSegment = newSegment;
        }   
        
        this->TotalSize += length + sizeof(SegmentHeader);     
        this->FreeSize += length + sizeof(SegmentHeader);     
    }

    SegmentHeader* Heap::GetSegmentHeader(void* address){
        return (SegmentHeader*)(void*)((uint64_t)address - sizeof(SegmentHeader));
    }
}
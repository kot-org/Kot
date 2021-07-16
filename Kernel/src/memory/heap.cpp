#include "heap.h"
#include "../paging/PageTableManager.h"
#include "../paging/PageFrameAllocator.h"

void* heapStart;
void* heapEnd;
HeapSegmentHeader* LastHdr;

//Heap globalHeap;

void InitializeHeap(void* heapAddress, size_t pageCount){
    void* pos = heapAddress;

    for (size_t i = 0; i < pageCount; i++){
        globalPageTableManager.MapMemory(pos, globalAllocator.RequestPage());
        pos = (void*)((size_t)pos + 0x1000);
    }

    size_t heapLength = pageCount * 0x1000;

    heapStart = heapAddress;
    heapEnd = (void*)((size_t)heapStart + heapLength);
    HeapSegmentHeader* startSeg = (HeapSegmentHeader*)heapAddress;
    startSeg->length = heapLength - sizeof(HeapSegmentHeader);
    startSeg->next = NULL;
    startSeg->last = NULL;
    startSeg->free = true;
    LastHdr = startSeg;
}

void freeK(void* address){
    HeapSegmentHeader* segment = (HeapSegmentHeader*)address - 1;
    segment->free = true;
    segment->CombineForward();
    segment->CombineBackward();

    uint64_t length = segment->length;

    memset(address, 0, length);
}

void* realloc(void* buffer, size_t size, uint64_t ajustement){
    void* newBuffer = mallocK(size);
    if(ajustement >= 0){
        memcpy(newBuffer, (void*)((uint64_t)buffer + ajustement), size - ajustement);
    }else{
        memcpy(newBuffer, buffer, size - ajustement);
    }
    
    freeK(buffer);
    return newBuffer;
}

void* mallocK(size_t size){
    if (size % 0x10 > 0){ // it is not a multiple of 0x10
        size -= (size % 0x10);
        size += 0x10;
    }

    if (size == 0) return NULL;

    HeapSegmentHeader* currentSeg = (HeapSegmentHeader*) heapStart;
    while(true){
        if(currentSeg->free){
            if (currentSeg->length > size){
                currentSeg->Split(size);
                currentSeg->free = false;
                return (void*)((uint64_t)currentSeg + sizeof(HeapSegmentHeader));
            }
            if (currentSeg->length == size){
                currentSeg->free = false;
                return (void*)((uint64_t)currentSeg + sizeof(HeapSegmentHeader));
            }
        }
        if (currentSeg->next == NULL) break;
        currentSeg = currentSeg->next;
    }
    ExpandHeap(size);
    return mallocK(size);
}

HeapSegmentHeader* HeapSegmentHeader::Split(size_t splitLength){
    if (splitLength < 0x10) return NULL;
    int64_t splitSegLength = length - splitLength - (sizeof(HeapSegmentHeader));
    if (splitSegLength < 0x10) return NULL;

    HeapSegmentHeader* newSplitHdr = (HeapSegmentHeader*) ((size_t)this + splitLength + sizeof(HeapSegmentHeader));
    next->last = newSplitHdr; // Set the next segment's last segment to our new segment
    newSplitHdr->next = next; // Set the new segment's next segment to out original next segment
    next = newSplitHdr; // Set our new segment to the new segment
    newSplitHdr->last = this; // Set our new segment's last segment to the current segment
    newSplitHdr->length = splitSegLength; // Set the new header's length to the calculated value
    newSplitHdr->free = free; // make sure the new segment's free is the same as the original
    length = splitLength; // set the length of the original segment to its new length

    if (LastHdr == this) LastHdr = newSplitHdr;
    return newSplitHdr;
}

void ExpandHeap(size_t length){
    if (length % 0x1000) {
        length -= length % 0x1000;
        length += 0x1000;
    }

    size_t pageCount = length / 0x1000;
    HeapSegmentHeader* newSegment = (HeapSegmentHeader*)heapEnd;

    for (size_t i = 0; i < pageCount; i++){
        globalPageTableManager.MapMemory(heapEnd, globalAllocator.RequestPage());
        heapEnd = (void*)((size_t)heapEnd + 0x1000);
    }

    newSegment->free = true;
    newSegment->last = LastHdr;
    LastHdr->next = newSegment;
    LastHdr = newSegment;
    newSegment->next = NULL;
    newSegment->length = length - sizeof(HeapSegmentHeader);
    newSegment->CombineBackward();

}

void HeapSegmentHeader::CombineForward(){
    if (next == NULL) return;
    if (!next->free) return;

    if (next == LastHdr) LastHdr = this;

    if (next->next != NULL){
        next->next->last = this;
    }

    length = length + next->length + sizeof(HeapSegmentHeader);

    next = next->next;
}

void HeapSegmentHeader::CombineBackward(){
    if (last != NULL && last->free) last->CombineForward();
}

/*void KInitializeHeap(void* heapAddress){
    globalHeap.heapLast = heapAddress;
}

void* Kalloc(size_t size){
    size += sizeof(HeapHeader);
    uint64_t TotalSize = size;
    if(size % 0x1000 > 0){
        TotalSize -= size % 0x1000;
        TotalSize += 0x1000;
    }

    uint64_t NumberOfPages = TotalSize / 0x1000;
    void* FirstAddress;

    for(int i = 0; i < NumberOfPages; i++){
        if(i == 0){
            ((HeapHeader*)globalHeap.heapLast)->length = TotalSize;
            FirstAddress = (void*)((uint64_t)globalHeap.heapLast + sizeof(HeapHeader));            
            if(heapStart == 0){
                heapStart = globalHeap.heapLast;
            }  
        }
        
        //globalPageTableManager.MapMemory(globalHeap.heapLast, globalAllocator.RequestPage());
        globalHeap.heapLast += 0x1000;
    }

    return FirstAddress;
}

void KFree(void* address){
    //get heap length
    void* BaseAddress = (void*)((uint64_t)address - sizeof(HeapHeader));
    HeapHeader* heapHeader = (HeapHeader*)BaseAddress; 
    uint64_t NumberOfPages = heapHeader->length / 0x1000;    

    for(int i = 0; i < NumberOfPages; i++){
        memset(BaseAddress, 0, 0x1000);
        globalPageTableManager.UnmapMememory(BaseAddress);        
    }
}*/
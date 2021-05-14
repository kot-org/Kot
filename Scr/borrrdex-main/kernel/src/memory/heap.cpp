#include "heap.h"
#include "KernelUtil.h"
#include "paging/PageTableManager.h"
#include "string.h"

static void* s_heap_start;
static void* s_heap_end;

extern "C" typedef struct heap_segment_hdr {
    size_t length;
    struct heap_segment_hdr* next;
    struct heap_segment_hdr* last;
    bool free;
} heap_segment_hdr_t;

static heap_segment_hdr_t* s_last_hdr;

void heap_combine_forward(heap_segment_hdr_t* hdr) {
    if(!hdr->next || !hdr->next->free) {
        return;
    }

    if(hdr->next == s_last_hdr) {
        s_last_hdr = hdr;
    }

    if(hdr->next->next) {
        hdr->next->next->last = hdr;
    }

    hdr->length = hdr->length + hdr->next->length + sizeof(heap_segment_hdr_t);
    hdr->next = hdr->next->next;
}

void heap_combine_backward(heap_segment_hdr_t* hdr) {
    if(hdr->last && hdr->last->free) {
        heap_combine_forward(hdr->last);
    }
}

heap_segment_hdr_t* heap_segment_split(heap_segment_hdr_t* hdr, size_t splitLength) {
    if(splitLength < 0x10) {
        return nullptr;
    }

    int64_t splitSegLength = hdr->length - splitLength - sizeof(heap_segment_hdr_t);
    if(splitSegLength < 0x10) {
        return nullptr;
    }

    heap_segment_hdr_t* newSplitHdr = (heap_segment_hdr_t *)((size_t)hdr + splitLength + sizeof(heap_segment_hdr_t));
    hdr->next->last = newSplitHdr;
    newSplitHdr->next = hdr->next;
    hdr->next = newSplitHdr;
    newSplitHdr->last = hdr;
    newSplitHdr->length = splitSegLength;
    newSplitHdr->free = hdr->free;
    hdr->length = splitLength;

    if(s_last_hdr == hdr) {
        s_last_hdr = newSplitHdr;
    }

    return newSplitHdr;
}

void heap_init(void* virtualAddress, size_t pageCount) {
    uint8_t* pos = (uint8_t *)virtualAddress;
    for(size_t i = 0; i < pageCount; i++, pos += 0x1000) {
        KernelPageTableManager()->MapMemory(pos, PageFrameAllocator::SharedAllocator()->RequestPage(), false);
    }

    s_heap_start = virtualAddress;
    s_heap_end = pos;
    heap_segment_hdr_t* startSeg = (heap_segment_hdr_t *)s_heap_start;
    startSeg->length = pageCount * 0x1000 - sizeof(heap_segment_hdr_t);
    startSeg->next = nullptr;
    startSeg->last = nullptr;
    startSeg->free = true;
    s_last_hdr = startSeg;
}

void heap_expand(size_t length) {
    size_t pageCount = (length + 0xfff) / 0x1000;
    heap_segment_hdr_t* newSegment = (heap_segment_hdr_t *)s_heap_end;
    
    uint8_t* pos = (uint8_t *)s_heap_end;
    for(size_t i = 0; i < pageCount; i++) {
        KernelPageTableManager()->MapMemory(pos, PageFrameAllocator::SharedAllocator()->RequestPage(), false);
        pos += 0x1000;
    }

    newSegment->free = true;
    newSegment->last = s_last_hdr;
    s_last_hdr->next = newSegment;
    s_last_hdr = newSegment;
    newSegment->next = nullptr;
    newSegment->length = (length + 0xfff) & ~0xfff;
    heap_combine_backward(newSegment);
}

void* kcalloc(size_t count, size_t itemsize) {
    void* retVal = kmalloc(count * itemsize);
    memset(retVal, 0, count * itemsize);
    return retVal;
}

void* kmalloc(size_t size) {
    size = (size + 0xf) & ~0xf;
    if(size == 0) {
        return nullptr;
    }

    heap_segment_hdr_t* currentSeg = (heap_segment_hdr_t *)s_heap_start;
    while(true) {
        if(currentSeg->free) {
            if(currentSeg->length >= size) {
                if(currentSeg->length > size) {
                    heap_segment_split(currentSeg, size);
                }

                currentSeg->free = false;
                return currentSeg + 1;
            }
        }

        if(!currentSeg->next) {
            break;
        }

        currentSeg = currentSeg->next;
    }

    heap_expand(size);
    return kmalloc(size);
}

void* krealloc(void* address, size_t size) {
    heap_segment_hdr_t* segment = (heap_segment_hdr_t *)address - 1;
    size_t originalSize = segment->length;
    if(segment->length + segment->next->length + sizeof(heap_segment_hdr_t) >= size) {
        heap_combine_forward(segment);

        // Best case, combining with the next free block gave us enough
        // memory at the same address, no copy needed
        if(segment->length > size) {
            heap_segment_split(segment, size);
        }

        segment->free = false;
        return segment + 1;
    }
    
    void* newAddr = kmalloc(size);
    if(!newAddr) {
        // Can't do it, don't touch the old block
        return nullptr;
    }

    heap_combine_backward(segment);
    segment->free = true;

    memcpy(newAddr, (void *)(segment + 1), originalSize);
    return newAddr;
}

void kfree(void* address) {
    heap_segment_hdr_t* segment = (heap_segment_hdr_t *)address - 1;
    segment->free = true;
    heap_combine_forward(segment);
    heap_combine_backward(segment);
}
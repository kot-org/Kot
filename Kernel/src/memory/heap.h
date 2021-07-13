#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../paging/pageTableManager.h"
#include "../paging/pageFrameAllocator.h"

struct HeapSegmentHeader{
    size_t length;
    HeapSegmentHeader* next;
    HeapSegmentHeader* last;
    bool free;
    void volatile CombineForward();
    void volatile CombineBackward();
    HeapSegmentHeader* volatile Split(size_t splitLenght);
};

void volatile InitializeHeap(void* heapAddress, size_t pageCount);

void* volatile mallocK(size_t size);
void* volatile realloc(void* buffer, size_t size, uint64_t ajustement);
void volatile freeK(void* address);

void volatile ExtendHeap(size_t lenght);

inline void* operator new(size_t size) {return mallocK(size);}
inline void* operator new[](size_t size) {return mallocK(size);}

inline void operator delete(void* address) {freeK(address);}
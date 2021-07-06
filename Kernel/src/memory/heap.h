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
    void CombineForward();
    void CombineBackward();
    HeapSegmentHeader* Split(size_t splitLenght);
};

void InitializeHeap(void* heapAddress, size_t pageCount);

void* mallocK(size_t size);
void* realloc(void* buffer, size_t size, uint64_t ajustement);
void free(void* address);

void ExtendHeap(size_t lenght);

inline void* operator new(size_t size) {return mallocK(size);}
inline void* operator new[](size_t size) {return mallocK(size);}

inline void operator delete(void* address) {free(address);}
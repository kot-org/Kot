#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../paging/pageTableManager.h"
#include "../paging/pageFrameAllocator.h"

/*struct HeapHeader
{
    uint64_t length;
};

struct Heap{
    size_t size;
    void* heapStart;
    void* heapLast;
};

void KInitializeHeap(void* heapAddress);
void* Kalloc(size_t size);
void KFree(void* address);*/


struct HeapSegmentHeader{
    size_t length;
    bool free;
    HeapSegmentHeader* next;
    HeapSegmentHeader* last;    
    void CombineForward();
    void CombineBackward();
    HeapSegmentHeader* Split(size_t splitLenght);
};

void InitializeHeap(void* heapAddress, size_t pageCount);

void* mallocK(size_t size);
void* realloc(void* buffer, size_t size, uint64_t ajustement);
void freeK(void* address);

void ExpandHeap(size_t lenght);

inline void* operator new(size_t size) {return mallocK(size);}
inline void* operator new[](size_t size) {return mallocK(size);}

inline void operator delete(void* address) {freeK(address);}
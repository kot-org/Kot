#pragma once
#include "../../lib/types.h"
#include "../../lib/stdio.h"
#include "../paging/pageTableManager.h"
#include "../paging/pageFrameAllocator.h"

struct SegmentHeader{
    size_t length;
    bool IsFree;
    bool IsUser;
    SegmentHeader* next;
    SegmentHeader* last;
}__attribute__((packed));

struct Heap{
    SegmentHeader* lastSegment = NULL;
    SegmentHeader* mainSegment = NULL;
    size_t TotalSize;
    size_t FreeSize;
    size_t UsedSize;
    void* heapEnd;
};

extern Heap globalHeap;

void InitializeHeap(void* heapAddress, size_t pageCount);

void* malloc(size_t size);
void* realloc(void* buffer, size_t size, uint64_t adjustement);
void free(void* address);

void ExpandHeap(size_t lenght);

inline void* operator new(size_t size) {return malloc(size);}
inline void* operator new[](size_t size) {return malloc(size);}

inline void operator delete(void* address) {free(address);}
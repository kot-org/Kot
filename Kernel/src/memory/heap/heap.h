#pragma once
#include "../../lib/types.h"
#include "../../lib/stdio.h"
#include "../paging/pageTableManager.h"
#include "../paging/pageFrameAllocator.h"
#include "../../logs/logs.h"

struct SegmentHeader{
    bool IsFree;
    size_t length;
    SegmentHeader* next;
    SegmentHeader* last;
}__attribute__((packed));

struct Heap{
    SegmentHeader* lastSegment = NULL;
    SegmentHeader* mainSegment = NULL;
    size_t TotalSize;
    size_t FreeSize;
    size_t UsedSize;
    void* heapEnd = 0;
};

extern Heap globalHeap;

void volatile InitializeHeap(void* heapAddress, size_t pageCount);

void* calloc(size_t size);
void* volatile malloc(size_t size);
void* volatile realloc(void* buffer, size_t size, uint64_t adjustement);
void volatile free(void* address);

void volatile SplitSegment(SegmentHeader* segment, size_t size);
void volatile ExpandHeap(size_t lenght);

inline void* operator new(size_t size) {return malloc(size);}
inline void* operator new[](size_t size) {return malloc(size);}

inline void operator delete(void* address) {free(address);}

SegmentHeader* GetSegmentHeader(void* address);
#pragma once

#include <kot/types.h>
#include <kot/heap.h>
#include <lib/stdio.h>
#include <logs/logs.h>
#include <arch/x86-64/cpu/cpu.h>
#include <arch/arch.h>

struct Heap{
    uintptr_t heapEnd = 0;
    SegmentHeader* lastSegment = NULL;
    SegmentHeader* mainSegment = NULL;
    size_t TotalSize;
    size_t FreeSize;
    size_t UsedSize;
    uint64_t lock;
};

extern Heap globalHeap;

void InitializeHeap(uintptr_t heapAddress, size_t pageCount);

uintptr_t calloc(size_t size);
uintptr_t malloc(size_t size);
uintptr_t realloc(uintptr_t buffer, size_t size);
void free(uintptr_t address);

SegmentHeader* SplitSegment(SegmentHeader* segment, size_t size);
void  ExpandHeap(size_t lenght);

inline uintptr_t operator new(size_t size) {return malloc(size);}
inline uintptr_t operator new[](size_t size) {return malloc(size);}

inline void operator delete(uintptr_t address) {free(address);}

SegmentHeader* GetSegmentHeader(uintptr_t address);
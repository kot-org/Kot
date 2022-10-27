#pragma once

#include <kot/types.h>
#include <kot/heap.h>
#include <lib/stdio.h>
#include <logs/logs.h>
#include <arch/arch.h>

struct Heap{
    uintptr_t heapEnd = 0;
    SegmentHeader* lastSegment = NULL;
    SegmentHeader* mainSegment = NULL;
    size64_t TotalSize;
    size64_t FreeSize;
    size64_t UsedSize;
    locker_t lock;
};

extern Heap globalHeap;

void InitializeHeap(uintptr_t heapAddress, size64_t pageCount);

uintptr_t calloc(size64_t size);
uintptr_t malloc(size64_t size);
uintptr_t realloc(uintptr_t buffer, size64_t size);
void free(uintptr_t address);

SegmentHeader* SplitSegment(SegmentHeader* segment, size64_t size);
void  ExpandHeap(size64_t lenght);

SegmentHeader* GetSegmentHeader(uintptr_t address);
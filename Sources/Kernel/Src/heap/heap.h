#pragma once

#include <settings.h>
#include <lib/types.h>
#include <lib/stdio.h>
#include <logs/logs.h>
#include <arch/arch.h>

struct Heap{
    uintptr_t lastStack;
    locker_t lockStack;
    uintptr_t heapEnd = 0;
    struct HeapSegmentHeader* lastSegment = NULL;
    struct HeapSegmentHeader* mainSegment = NULL;
    size64_t TotalSize;
    size64_t FreeSize;
    size64_t UsedSize;
    locker_t lock;
};

#ifdef HEAP_DEBUG
struct HeapSegmentHeader{
    size64_t length;
}__attribute__((aligned(0x10)));
#else
struct HeapSegmentHeader{
    bool IsFree;
    size64_t length;
    struct HeapSegmentHeader* next;
    struct HeapSegmentHeader* last;
    bool IsStack;
    uint32_t signature;
}__attribute__((aligned(0x10)));
#endif

extern Heap globalHeap;

void InitializeHeap(uintptr_t heapAddress, uintptr_t stackAddress, size64_t pageCount);

uintptr_t kcalloc(size64_t size);
uintptr_t kmalloc(size64_t size);
uintptr_t krealloc(uintptr_t buffer, size64_t size);

void kfree(uintptr_t address);

uintptr_t stackalloc(size64_t size);

HeapSegmentHeader* SplitSegment(HeapSegmentHeader* segment, size64_t size);
void  ExpandHeap(size64_t lenght);

HeapSegmentHeader* GetHeapSegmentHeader(uintptr_t address);
#pragma once

#include <settings.h>
#include <lib/types.h>
#include <lib/stdio.h>
#include <logs/logs.h>
#include <arch/arch.h>

struct Heap{
    void* lastStack;
    locker_t lockStack;
    void* heapEnd = 0;
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

void InitializeHeap(void* heapAddress, void* stackAddress, size64_t pageCount);

void* kcalloc(size64_t size);
void* kmalloc(size64_t size);
void* krealloc(void* buffer, size64_t size);

void kfree(void* address);

void* stackalloc(size64_t size);

HeapSegmentHeader* SplitSegment(HeapSegmentHeader* segment, size64_t size);
void  ExpandHeap(size64_t lenght);

HeapSegmentHeader* GetHeapSegmentHeader(void* address);
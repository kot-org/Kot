#pragma once
#include <kot/types.h>
#include <kot/memory.h>
#include <kot/atomic.h>

struct SegmentHeader{
    bool IsFree;
    size_t length;
    struct SegmentHeader* next;
    struct SegmentHeader* last;
    uint8_t singature;
}__attribute__((packed));

struct Heap{
    struct SegmentHeader* lastSegment;
    struct SegmentHeader* mainSegment;
    size_t TotalSize;
    size_t FreeSize;
    size_t UsedSize;
    void* heapEnd;
    uint64_t lock;
};

extern struct Heap globalHeap;

void InitializeHeap(void* heapAddress, size_t pageCount);

void* calloc(size_t size);
void* malloc(size_t size);
void* realloc(void* buffer, size_t size);
void free(void* address);

void  SplitSegment(struct SegmentHeader* segment, size_t size);
void  ExpandHeap(size_t lenght);

struct SegmentHeader* GetSegmentHeader(void* address);
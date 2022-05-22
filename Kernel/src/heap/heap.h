#pragma once
#include <kot/types.h>
#include <lib/stdio.h>
#include <logs/logs.h>
#include <arch/x86-64/cpu/cpu.h>
#include <arch/arch.h>

struct SegmentHeader{
    bool IsFree;
    size_t length;
    SegmentHeader* next;
    SegmentHeader* last;
    uint8_t singature;
}__attribute__((packed));

struct Heap{
    void* heapEnd = 0;
    SegmentHeader* lastSegment = NULL;
    SegmentHeader* mainSegment = NULL;
    size_t TotalSize;
    size_t FreeSize;
    size_t UsedSize;
    uint64_t lock;
};

extern Heap globalHeap;

void InitializeHeap(void* heapAddress, size_t pageCount);

void* calloc(size_t size);
void* malloc(size_t size);
void* realloc(void* buffer, size_t size);
void free(void* address);

SegmentHeader*  SplitSegment(SegmentHeader* segment, size_t size);
void  ExpandHeap(size_t lenght);

inline void* operator new(size_t size) {return malloc(size);}
inline void* operator new[](size_t size) {return malloc(size);}

inline void operator delete(void* address) {free(address);}

SegmentHeader* GetSegmentHeader(void* address);
#ifndef _HEAP_H
#define _HEAP_H 1

#include <kot/sys.h>
#include <kot/types.h>
#include <kot/memory.h>

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
    uint64_t heapEnd;
    kprocess_t process;
};

#if defined(__cplusplus)
extern "C" {
#endif

void InitializeHeap();
void* calloc(size_t size);
void* malloc(size_t size);
void* realloc(void* buffer, size_t size);
void free(void* address);
void  SplitSegment(struct SegmentHeader* segment, size_t size);
void  ExpandHeap(size_t lenght);
struct SegmentHeader* GetSegmentHeader(void* address);

#if defined(__cplusplus)
}
#endif

#endif
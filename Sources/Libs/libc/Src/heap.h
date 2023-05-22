#ifndef _HEAP_H
#define _HEAP_H 1

#include <kot/sys.h>
#include <kot/types.h>
#include <kot/memory.h>

struct SegmentHeader{
    bool IsFree;
    size64_t length;
    struct SegmentHeader* next;
    struct SegmentHeader* last;
    uint32_t signature;
}__attribute__((aligned(0x10)));

struct heap_t{
    struct SegmentHeader* lastSegment;
    struct SegmentHeader* mainSegment;
    size64_t TotalSize;
    size64_t FreeSize;
    size64_t UsedSize;
    uint64_t EndAddress;
    process_t Process;
    bool IsHeapEnabled;
}__attribute__((packed));

extern struct heap_t heap;

#if defined(__cplusplus)
extern "C" {
#endif

void InitializeHeapUser();
void* calloc(size64_t size);
void* malloc(size64_t size);
void* realloc(void* buffer, size64_t size);
void free(void* address);
void SplitSegmentUser(struct SegmentHeader* segment, size64_t size);
void ExpandHeapUser(size64_t lenght);
struct SegmentHeader* GetSegmentHeaderUser(void* address);

void MergeNextAndThisToLastUser(struct SegmentHeader* header);
void MergeThisToLastUser(struct SegmentHeader* header);
void MergeNextToThisUser(struct SegmentHeader* header);

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)
inline void* operator new(size_t Size){
    return malloc(Size);
}
inline void* operator new[](size_t Size){
    return malloc(Size);
}

inline void operator delete(void* p){
    free(p);
}
inline void operator delete[](void* p){
    free(p);
}
#endif

#endif
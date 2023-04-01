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
uintptr_t calloc(size64_t size);
uintptr_t malloc(size64_t size);
uintptr_t realloc(uintptr_t buffer, size64_t size);
void free(uintptr_t address);
void SplitSegmentUser(struct SegmentHeader* segment, size64_t size);
void ExpandHeapUser(size64_t lenght);
struct SegmentHeader* GetSegmentHeaderUser(uintptr_t address);

void MergeNextAndThisToLastUser(struct SegmentHeader* header);
void MergeThisToLastUser(struct SegmentHeader* header);
void MergeNextToThisUser(struct SegmentHeader* header);

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)
inline uintptr_t operator new(size_t Size){
    return malloc(Size);
}
inline uintptr_t operator new[](size_t Size){
    return malloc(Size);
}

inline void operator delete(uintptr_t p){
    free(p);
}
inline void operator delete[](uintptr_t p){
    free(p);
}
#endif

#endif
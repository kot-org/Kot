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
    uint32_t signature;
}__attribute__((aligned(0x10)));

struct heap_t{
    struct SegmentHeader* lastSegment;
    struct SegmentHeader* mainSegment;
    size_t TotalSize;
    size_t FreeSize;
    size_t UsedSize;
    uint64_t EndAddress;
    kprocess_t Process;
    bool IsHeapEnabled;
}__attribute__((packed));

#if defined(__cplusplus)
extern "C" {
#endif

void InitializeHeapUser();
uintptr_t calloc(size_t size);
uintptr_t malloc(size_t size);
uintptr_t realloc(uintptr_t buffer, size_t size);
void free(uintptr_t address);
void SplitSegmentUser(struct SegmentHeader* segment, size_t size);
void ExpandHeapUser(size_t lenght);
struct SegmentHeader* GetSegmentHeaderUser(uintptr_t address);

void MergeNextAndThisToLastUser(struct SegmentHeader* header);
void MergeThisToLastUser(struct SegmentHeader* header);
void MergeNextToThisUser(struct SegmentHeader* header);

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)
inline void* operator new(size_t Size) {return malloc(Size);}
inline void* operator new[](size_t Size) {return malloc(Size);}

inline void operator delete(void* p) {free(p);}
#endif

#endif
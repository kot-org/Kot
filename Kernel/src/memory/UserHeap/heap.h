#pragma once
#include "../../lib/types.h"
#include "../../lib/stdio.h"
#include "../paging/pageTableManager.h"
#include "../paging/pageFrameAllocator.h"
#include "../../logs/logs.h"

namespace UserHeap{
    struct SegmentHeader{
        bool IsFree;
        size_t length;
        SegmentHeader* next;
        SegmentHeader* last;
    }__attribute__((packed));

    struct Heap{
        PageTableManager* UserPageTable;
        SegmentHeader* lastSegment = NULL;
        SegmentHeader* mainSegment = NULL;
        size_t TotalSize;
        size_t FreeSize;
        size_t UsedSize;
        void* heapEnd = 0;

        void* calloc(size_t size);
        void* malloc(size_t size);
        void free(void* address);
        void* realloc(void* buffer, size_t size, uint64_t adjustement);

        void MergeNextAndThisToLast(SegmentHeader* header);
        void MergeThisToLast(SegmentHeader* header);
        void MergeNextToThis(SegmentHeader* header);

        void SplitSegment(SegmentHeader* segment, size_t size);
        void ExpandHeap(size_t length);
        SegmentHeader* GetSegmentHeader(void* address);
    };

    Heap* InitializeHeap(void* heapAddress, size_t pageCount, PageTableManager* UserPageTable);
}
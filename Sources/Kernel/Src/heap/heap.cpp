#include <heap/heap.h>
#include <arch/arch.h>

Heap globalHeap;

void InitializeHeap(void* heapAddress, void* stackAddress, size64_t pageCount){
    globalHeap.heapEnd = heapAddress;
    globalHeap.lastStack = stackAddress;
    AtomicClearLock(&globalHeap.lock);
    #ifndef HEAP_DEBUG
    void* NewPhysicalAddress = Pmm_RequestPage();
    globalHeap.heapEnd = (void*)((uint64_t)globalHeap.heapEnd - PAGE_SIZE);
    vmm_Map(vmm_PageTable, globalHeap.heapEnd, NewPhysicalAddress, false, true, true);
    globalHeap.mainSegment = (HeapSegmentHeader*)((uint64_t)globalHeap.heapEnd + ((uint64_t)PAGE_SIZE - sizeof(HeapSegmentHeader)));
    globalHeap.mainSegment->signature = 0xff;
    globalHeap.mainSegment->length = 0;
    globalHeap.mainSegment->IsFree = false;
    globalHeap.mainSegment->IsStack = false;
    globalHeap.mainSegment->last = NULL;
    globalHeap.mainSegment->next = (HeapSegmentHeader*)((uint64_t)globalHeap.mainSegment - (uint64_t)PAGE_SIZE + sizeof(HeapSegmentHeader));

    globalHeap.mainSegment->next->IsFree = true;
    globalHeap.mainSegment->next->IsStack = false;
    globalHeap.mainSegment->next->length = (uint64_t)PAGE_SIZE - sizeof(HeapSegmentHeader) - sizeof(HeapSegmentHeader); /* remove twice because we have the main and new header in the same page */
    globalHeap.mainSegment->next->signature = 0xff;
    globalHeap.mainSegment->next->last = globalHeap.mainSegment;
    globalHeap.mainSegment->next->next = NULL;
    globalHeap.lastSegment = globalHeap.mainSegment->next;  

    globalHeap.TotalSize += PAGE_SIZE;     
    globalHeap.FreeSize += PAGE_SIZE;  
    #endif
}

void* _kcalloc(size64_t size){
    void* address = kmalloc(size);
    memset(address, 0, size);
    return address;
}

#ifdef HEAP_TRACE
void* _kcallocd(size64_t size, void* data){
    void* address = _kcalloc(size);
    GetHeapSegmentHeader(address)->TraceData = data;
    return address;
}
#endif

void* _kmalloc(size64_t size){
    #ifdef HEAP_DEBUG
    if(size == 0) return NULL;
    AtomicAcquire(&globalHeap.lock);
    size64_t SizeToAlloc = size + sizeof(HeapSegmentHeader);
    if(SizeToAlloc % PAGE_SIZE){
        SizeToAlloc -= SizeToAlloc % PAGE_SIZE;
        SizeToAlloc += PAGE_SIZE;
    }
    HeapSegmentHeader* currentSeg = (HeapSegmentHeader*)((uint64_t)globalHeap.heapEnd - size - sizeof(HeapSegmentHeader));
    void* Buffer = (void*)((uint64_t)globalHeap.heapEnd - size);
    globalHeap.heapEnd = (void*)((uint64_t)globalHeap.heapEnd - SizeToAlloc - HEAP_DEBUG_GUARD_SIZE); // Add page size for the guard
    assert(globalHeap.heapEnd < globalHeap.lastStack);
    for(size64_t i = 0; i < SizeToAlloc; i += PAGE_SIZE){
        vmm_Map((void*)((uint64_t)currentSeg + i), Pmm_RequestPage());
    }
    currentSeg->length = SizeToAlloc;
    AtomicRelease(&globalHeap.lock);
    return Buffer;
    #else
    if(size == 0) return NULL;

    if(size % 0x10 > 0){ // it is not a multiple of 0x10
        size -= (size % 0x10);
        size += 0x10;
    }

    AtomicAcquire(&globalHeap.lock);
    HeapSegmentHeader* currentSeg = (HeapSegmentHeader*)globalHeap.mainSegment;
    uint64_t SizeWithHeader = size + sizeof(HeapSegmentHeader);
    while(true){
        if(currentSeg->IsFree){
            if(currentSeg->length > SizeWithHeader){
                // split this segment in two 
                currentSeg = SplitSegment(currentSeg, size);
                currentSeg->IsFree = false;
                globalHeap.UsedSize += currentSeg->length + sizeof(HeapSegmentHeader);
                globalHeap.FreeSize -= currentSeg->length + sizeof(HeapSegmentHeader);
                AtomicRelease(&globalHeap.lock);
                return (void*)((uint64_t)currentSeg + sizeof(HeapSegmentHeader));
            }else if(currentSeg->length == size){
                currentSeg->IsFree = false;
                globalHeap.UsedSize += currentSeg->length + sizeof(HeapSegmentHeader);
                globalHeap.FreeSize -= currentSeg->length + sizeof(HeapSegmentHeader);
                AtomicRelease(&globalHeap.lock);
                return (void*)((uint64_t)currentSeg + sizeof(HeapSegmentHeader));
            }
        }
        if(currentSeg->next == NULL) break;
        currentSeg = currentSeg->next;
    }
    
    ExpandHeap(size);
    AtomicRelease(&globalHeap.lock);
    return kmalloc(size);
    #endif
}

#ifdef HEAP_TRACE
void* _kmallocd(size64_t size, void* data){
    void* address = _kmalloc(size);
    GetHeapSegmentHeader(address)->TraceData = data;
    return address;
}
#endif

#ifndef HEAP_DEBUG
void MergeThisToNext(HeapSegmentHeader* header){
    // merge this segment into the last segment
    HeapSegmentHeader* headerNext = header->next;
    headerNext->length += header->length + sizeof(HeapSegmentHeader);
    headerNext->last = header->last;
    header->last->next = headerNext;

    memset(header, 0, sizeof(HeapSegmentHeader));
}
#endif

#ifndef HEAP_DEBUG
void MergeLastToThis(HeapSegmentHeader* header){
    // merge this segment into the next segment

    HeapSegmentHeader* headerLast = header->last;
    header->length += headerLast->length + sizeof(HeapSegmentHeader);
    header->last = headerLast->last;
    header->last->next = header;

    memset(headerLast, 0, sizeof(HeapSegmentHeader));
}
#endif

#ifndef HEAP_DEBUG
void MergeLastAndThisToNext(HeapSegmentHeader* header){
    // merge this segment into the last segment
    MergeLastToThis(header);
    MergeThisToNext(header);
}
#endif


void kfree(void* address){
    if(address != NULL){
        #ifdef HEAP_DEBUG
        HeapSegmentHeader* header = (HeapSegmentHeader*)(void*)((uint64_t)address - sizeof(HeapSegmentHeader));
        size64_t SizeToFree = header->length;
        for(size64_t i = 0; i < SizeToFree; i += PAGE_SIZE){
            void* PhysicalPage = vmm_GetPhysical((void*)((uint64_t)header + i));
            vmm_Unmap((void*)((uint64_t)header + i));
            Pmm_FreePage(PhysicalPage);
        }
        #else
        AtomicAcquire(&globalHeap.lock);
        HeapSegmentHeader* header = (HeapSegmentHeader*)(void*)((uint64_t)address - sizeof(HeapSegmentHeader));
        header->IsFree = true;
        globalHeap.FreeSize += header->length + sizeof(HeapSegmentHeader);
        globalHeap.UsedSize -= header->length + sizeof(HeapSegmentHeader);

        if(header->next != NULL && header->last != NULL){
            if(header->next->IsFree && header->last->IsFree){
                // merge this segment and next segment into the last segment
                MergeLastAndThisToNext(header);
                AtomicRelease(&globalHeap.lock);
                return;
            }
        }

        if(header->last != NULL){
            if(header->last->IsFree){
                // merge this segment into the last segment
                MergeLastToThis(header);
                AtomicRelease(&globalHeap.lock);  
                return;  
            }         
        }
        
        if(header->next != NULL){
            if(header->next->IsFree){
                // merge this segment into the next segment
                MergeThisToNext(header);
                AtomicRelease(&globalHeap.lock);
                return; 
            }
        }
        AtomicRelease(&globalHeap.lock);
        #endif
    }
}

void* _krealloc(void* buffer, size64_t size){
    void* newBuffer = kmalloc(size);
    if(newBuffer == NULL){
        return NULL;
    }
    if(buffer != NULL){
        size64_t oldSize = GetHeapSegmentHeader(buffer)->length;
        if (size < oldSize) {
            oldSize = size;
        }
        memcpy(newBuffer, buffer, oldSize);
        kfree(buffer);
    }
    return newBuffer;
}

#ifdef HEAP_TRACE
void* _kreallocd(void* buffer, size64_t size, void* data){
    void* address = _krealloc(buffer, size);
    GetHeapSegmentHeader(address)->TraceData = data;
    return address;
}
#endif

#ifndef HEAP_DEBUG
HeapSegmentHeader* SplitSegment(HeapSegmentHeader* segment, size64_t size){
    if(segment->length > size + sizeof(HeapSegmentHeader)){
        HeapSegmentHeader* newSegment = (HeapSegmentHeader*)(void*)((uint64_t)segment + segment->length - size);
        memset(newSegment, 0, sizeof(HeapSegmentHeader));
        newSegment->IsFree = true;      
        newSegment->IsStack = false;      
        newSegment->signature = 0xff;       
        newSegment->length = size;
        newSegment->next = segment;
        newSegment->last = segment->last;

        if(segment->next == NULL){
            globalHeap.lastSegment = segment;
        }

        if(segment->last != NULL){
            segment->last->next = newSegment;
        }
        segment->last = newSegment;
        segment->length = segment->length - (size + sizeof(HeapSegmentHeader));  
        return newSegment;      
    }
    return NULL;
}
#endif

#ifndef HEAP_DEBUG
void ExpandHeap(size64_t length){
    length += sizeof(HeapSegmentHeader);
    if(length % PAGE_SIZE){
        length -= length % PAGE_SIZE;
        length += PAGE_SIZE;
    }

    size64_t pageCount = DivideRoundUp(length, PAGE_SIZE);


    for (size64_t i = 0; i < pageCount; i++){
        void* NewPhysicalAddress = Pmm_RequestPage();
        globalHeap.heapEnd = (void*)((uint64_t)globalHeap.heapEnd - (uint64_t)PAGE_SIZE);
        vmm_Map(vmm_PageTable, globalHeap.heapEnd, NewPhysicalAddress, false, true, true);
    }

    HeapSegmentHeader* newSegment = (HeapSegmentHeader*)globalHeap.heapEnd;

    if(globalHeap.lastSegment != NULL && globalHeap.lastSegment->IsFree && globalHeap.lastSegment->last != NULL){
        uint64_t size = globalHeap.lastSegment->length + length;
        newSegment->signature = 0xff;
        newSegment->length = size - sizeof(HeapSegmentHeader);
        newSegment->IsFree = true;
        newSegment->IsStack = false;
        newSegment->last = globalHeap.lastSegment->last;
        newSegment->last->next = newSegment;
        newSegment->next = NULL;
        globalHeap.lastSegment = newSegment;    
    }else{
        newSegment->signature = 0xff;
        newSegment->length = length - sizeof(HeapSegmentHeader);
        newSegment->IsFree = true;
        newSegment->IsStack = false;
        newSegment->last = globalHeap.lastSegment;
        newSegment->next = NULL;
        if(globalHeap.lastSegment != NULL){
            globalHeap.lastSegment->next = newSegment;
        }
        globalHeap.lastSegment = newSegment;        
    }  
    
    globalHeap.TotalSize += length + sizeof(HeapSegmentHeader);     
    globalHeap.FreeSize += length + sizeof(HeapSegmentHeader);     
}
#endif

void* stackalloc(size64_t size){
    AtomicAcquire(&globalHeap.lockStack);
    size64_t pageCount = DivideRoundUp(size, PAGE_SIZE);

    globalHeap.lastStack = (void*)((uint64_t)globalHeap.lastStack - (uint64_t)PAGE_SIZE);

    for (size64_t i = 0; i < pageCount; i++){
        void* NewPhysicalAddress = Pmm_RequestPage();
        globalHeap.lastStack = (void*)((uint64_t)globalHeap.lastStack - (uint64_t)PAGE_SIZE);
        vmm_Map(vmm_PageTable, globalHeap.lastStack, NewPhysicalAddress, false, true, true);
    }

    void* Address = globalHeap.lastStack;

    globalHeap.lastStack = (void*)((uint64_t)globalHeap.lastStack - (uint64_t)PAGE_SIZE);

    AtomicRelease(&globalHeap.lockStack);

    return Address;
}

HeapSegmentHeader* GetHeapSegmentHeader(void* address){
    return (HeapSegmentHeader*)(void*)((uint64_t)address - sizeof(HeapSegmentHeader));
}
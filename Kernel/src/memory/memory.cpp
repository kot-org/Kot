#include "memory.h"
#include "../logs/logs.h"
#include "../arch/x86-64/gdt/gdt.h"


uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t mMapDescSize){
    static uint64_t memorySizeBytes = 0;
    if (memorySizeBytes > 0) return memorySizeBytes;

    for (int i = 0; i < mMapEntries; i++){
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescSize));
        memorySizeBytes += desc->numPages * 0x1000;
    }

    return memorySizeBytes;
}

void memset(void* start, uint8_t value, uint64_t num){
    for (uint64_t i = 0; i < num; i++){
        *(uint8_t*)((uint64_t)start + i) = value;
    }
}

void memcpy(void* destination, void* source, uint64_t num){
    long d0, d1, d2; 
    asm volatile(
            "rep ; movsq\n\t movq %4,%%rcx\n\t""rep ; movsb\n\t": "=&c" (d0),
            "=&D" (d1),
            "=&S" (d2): "0" (num >> 3), 
            "g" (num & 7), 
            "1" (destination),
            "2" (source): "memory"
    );  
}

int memcmp(const void *aptr, const void *bptr, size_t n){
	const unsigned char *a = (const unsigned char*)aptr, *b = (const unsigned char*)bptr;
	for (size_t i = 0; i < n; i++) {
		if (a[i] < b[i])
			return -1;
		else if (a[i] > b[i])
			return 1;
	}
	return 0;
}

namespace Memory{
    //PT_Flag::Custom1 master share
    //PT_Flag::Custom2 slave share
    
    void* CreatSharing(PageTableManager* pageTable, size_t size, void* virtualAddress, uint8_t Priviledge){
        if((uint64_t)virtualAddress % 0x1000 > 0){
            virtualAddress -= (uint64_t)virtualAddress % 0x1000;
            virtualAddress += 0x1000;
        }
        uint64_t realSize = size + sizeof(MemoryShareInfo);
        uint64_t numberOfPage = Divide(realSize, 0x1000);
        for(int i = 0; i < numberOfPage; i++){
            uint64_t virtualAddressIterator = (uint64_t)virtualAddress + i * 0x1000;
            if(!pageTable->GetFlags((void*)virtualAddressIterator, PT_Flag::Present)){
                pageTable->MapMemory((void*)virtualAddressIterator, globalAllocator.RequestPage());
                pageTable->SetFlags((void*)virtualAddressIterator, PT_Flag::Custom1, true); //set master state
            }
            if(Priviledge == UserAppRing) pageTable->MapUserspaceMemory((void*)virtualAddressIterator);
        }
        MemoryShareInfo* shareInfo = (MemoryShareInfo*)virtualAddress;
        shareInfo->Lock = false;
        shareInfo->Size = realSize;
        shareInfo->PageNumber = numberOfPage;
        shareInfo->PageTableParent = pageTable;
        shareInfo->VirtualAddressParent = virtualAddress;
        void* key = pageTable->GetPhysicalAddress(virtualAddress);
        shareInfo = (MemoryShareInfo*)pageTable->GetVirtualAddress(key);
        return key;
    }

    bool GetSharing(PageTableManager* pageTable, void* key, void* virtualAddress, uint8_t Priviledge){
        if((uint64_t)virtualAddress % 0x1000 > 0){
            virtualAddress -= (uint64_t)virtualAddress % 0x1000;
            virtualAddress += 0x1000;
        }
        MemoryShareInfo* shareInfo = (MemoryShareInfo*)pageTable->GetVirtualAddress(key);
        for(uint64_t i = 0; i < shareInfo->PageNumber; i++){
            uint64_t virtualAddressIterator = (uint64_t)virtualAddress + i * 0x1000;
            uint64_t virtualAddressParentIterator = (uint64_t)shareInfo->VirtualAddressParent + i * 0x1000;
            void* physicalAddressParentIterator = shareInfo->PageTableParent->GetPhysicalAddress((void*)virtualAddressParentIterator);
            pageTable->MapMemory((void*)virtualAddressIterator, physicalAddressParentIterator);
            pageTable->SetFlags((void*)virtualAddressIterator, PT_Flag::Custom2, true); //set slave state
            if(Priviledge == UserAppRing) pageTable->MapUserspaceMemory((void*)virtualAddressIterator);
        }
        return true;
    }
}
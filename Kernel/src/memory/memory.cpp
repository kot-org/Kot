#include "memory.h"
#include "../logs/logs.h"
#include "../arch/x86-64/gdt/gdt.h"

static uint64_t mutexMemory;

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
    /* _____________________________Stack Creation___________________________ */
    void* CreatStack(PageTableManager* pageTable, size_t PageNumber, bool IsUser){
        //first free page forstack is higher half - 0x1000
        uint64_t virtualAddressIterator = HigherHalfAddress - 0x1000;
        //find free pages
        while(pageTable->GetFlags((void*)virtualAddressIterator, PT_Flag::Present)){
            virtualAddressIterator -= 0x1000;
        }  

        void* stackAddress = (void*)virtualAddressIterator;
        //allocate pages
        for(int i = 0; i < PageNumber; i++){
            pageTable->MapMemory((void*)virtualAddressIterator, (void*)globalAllocator.RequestPage());
            if(IsUser) pageTable->MapUserspaceMemory((void*)virtualAddressIterator);
            virtualAddressIterator -= 0x1000;
        }

        return stackAddress;
    }

    /* _____________________________Share Memory_____________________________ */
    //PT_Flag::Custom1 master share
    //PT_Flag::Custom2 slave share
    
    size_t CreatSharing(PageTableManager* pageTable, size_t size, uint64_t* virtualAddressPointer, uint64_t* keyPointer, bool ReadOnly, uint8_t Priviledge){
        void* virtualAddress = (void*)*virtualAddressPointer;
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
        shareInfo->ReadOnly = ReadOnly;
        shareInfo->Size = realSize;
        shareInfo->PageNumber = numberOfPage;
        shareInfo->PageTableParent = pageTable;
        shareInfo->VirtualAddressParent = virtualAddress;
        void* key = pageTable->GetPhysicalAddress(virtualAddress);
        shareInfo = (MemoryShareInfo*)pageTable->GetVirtualAddress(key);
        *virtualAddressPointer = (uint64_t)virtualAddress;
        *keyPointer = (uint64_t)key;
        return numberOfPage * 0x1000;
    }

    bool GetSharing(PageTableManager* pageTable, void* key, uint64_t* virtualAddressPointer, uint8_t Priviledge){
        void* virtualAddress = (void*)*virtualAddressPointer;
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
            if(shareInfo->ReadOnly) pageTable->SetFlags((void*)virtualAddressIterator, PT_Flag::ReadWrite, false); 
        }

        *virtualAddressPointer = (uint64_t)virtualAddress;
        return true;
    }

    size_t FreeSharing(void* virtualAddress){
        MemoryShareInfo* shareInfo = (MemoryShareInfo*)virtualAddress;
        PageTableManager* pageTable = shareInfo->PageTableParent;
        size_t NumberOfPage = shareInfo->PageNumber;

        for(uint64_t i = 0; i < NumberOfPage; i++){
            uint64_t virtualAddressIterator = (uint64_t)virtualAddress + i * 0x1000;
            void* physcialAddress = pageTable->GetPhysicalAddress((void*)virtualAddressIterator);
            globalAllocator.FreePage(physcialAddress);
            pageTable->UnmapMemory((void*)virtualAddressIterator);
        }

        return NumberOfPage * 0x1000;
    }
}
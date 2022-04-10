#include <logs/logs.h>
#include <memory/memory.h>
#include <arch/x86-64/gdt/gdt.h>

static uint64_t mutexMemory;

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

bool CheckAddress(void* address, size_t size){
    uint64_t NumberPage = Divide(size, 0x1000);
    uint64_t AddressItinerator = (uint64_t)address;
    void* PagingEntry = NULL;
    __asm__ __volatile__ ("mov %%cr3, %%rax" : "=a"(PagingEntry));

    for(int i = 0; i < NumberPage; i++){
        if(!GetFlags(PagingEntry, (void*)AddressItinerator, PT_Flag::Present)) return false;
        AddressItinerator += 0x1000;
    }

    return true;
}


/* _____________________________Share Memory_____________________________ */
//PT_Flag::Custom1 master share
//PT_Flag::Custom2 slave share

uint64_t CreatSharing(thread_t* thread, size_t size, uint64_t* virtualAddressPointer, uint64_t* keyPointer, bool ReadOnly){
    PageTableManager* pageTable = thread->Paging;
    void* virtualAddress = (void*)*virtualAddressPointer;
    if((uint64_t)virtualAddress % 0x1000 > 0){
        virtualAddress -= (uint64_t)virtualAddress % 0x1000;
        virtualAddress += 0x1000;
    }
    uint64_t realSize = size;
    uint64_t numberOfPage = Divide(realSize, 0x1000);
    for(int i = 0; i < numberOfPage; i++){
        uint64_t virtualAddressIterator = (uint64_t)virtualAddress + i * 0x1000;
        if(!pageTable->GetFlags((void*)virtualAddressIterator, PT_Flag::Present)){
            pageTable->MapMemory((void*)virtualAddressIterator, globalAllocator.RequestPage());
            pageTable->SetFlags((void*)virtualAddressIterator, PT_Flag::Custom1, true); //set master state
        }
        if(thread->RingPL == UserAppRing) pageTable->MapUserspaceMemory((void*)virtualAddressIterator);
    }
    MemoryShareInfo* shareInfo = (MemoryShareInfo*)malloc(sizeof(MemoryShareInfo));
    shareInfo->Lock = false;
    shareInfo->ReadOnly = ReadOnly;
    shareInfo->Size = realSize;
    shareInfo->PageNumber = numberOfPage;
    shareInfo->PageTableParent = pageTable;
    shareInfo->VirtualAddressParent = virtualAddress;
    shareInfo->signature0 = 'S';
    shareInfo->signature1 = 'M';
    void* key = pageTable->GetPhysicalAddress(virtualAddress);
    shareInfo = (MemoryShareInfo*)pageTable->GetVirtualAddress(key);
    *virtualAddressPointer = (uint64_t)virtualAddress;
    *keyPointer = (uint64_t)shareInfo;

    thread->MemoryAllocated += numberOfPage * 0x1000;
    return KSUCCESS;
}

uint64_t GetSharing(thread_t* thread, uint64_t key, uint64_t* virtualAddressPointer){
    MemoryShareInfo* shareInfo = (MemoryShareInfo*)key;
    PageTableManager* pageTable = thread->Paging;
    void* virtualAddress = (void*)*virtualAddressPointer;
    if((uint64_t)virtualAddress % 0x1000 > 0){
        virtualAddress -= (uint64_t)virtualAddress % 0x1000;
        virtualAddress += 0x1000;
    }
    
    if(shareInfo->signature0 != 'S' || shareInfo->signature1 != 'M') return false;
    for(uint64_t i = 0; i < shareInfo->PageNumber; i++){
        uint64_t virtualAddressIterator = (uint64_t)virtualAddress + i * 0x1000;
        uint64_t virtualAddressParentIterator = (uint64_t)shareInfo->VirtualAddressParent + i * 0x1000;
        void* physicalAddressParentIterator = shareInfo->PageTableParent->GetPhysicalAddress((void*)virtualAddressParentIterator);
        pageTable->MapMemory((void*)virtualAddressIterator, physicalAddressParentIterator);
        pageTable->SetFlags((void*)virtualAddressIterator, PT_Flag::Custom2, true); //set slave state
        if(thread->RingPL) pageTable->MapUserspaceMemory((void*)virtualAddressIterator);
        if(shareInfo->ReadOnly) pageTable->SetFlags((void*)virtualAddressIterator, PT_Flag::ReadWrite, false); 
    }
    *virtualAddressPointer = (uint64_t)virtualAddress;
    return KSUCCESS;
}

uint64_t FreeSharing(thread_t* thread, uint64_t key){
    PageTableManager* pageTable = thread->Paging;
    MemoryShareInfo* shareInfo = (MemoryShareInfo*)key;
    void* virtualAddress = (void*)key;
    PageTableManager* pageTableMaster = shareInfo->PageTableParent;
    size_t NumberOfPage = shareInfo->PageNumber;
    for(uint64_t i = 0; i < NumberOfPage; i++){
        uint64_t virtualAddressIterator = (uint64_t)virtualAddress + i * 0x1000;
        if(pageTable->GetFlags((void*)virtualAddressIterator, PT_Flag::Custom1)){ // is master
            void* physcialAddress = pageTable->GetPhysicalAddress((void*)virtualAddressIterator);
            globalAllocator.FreePage(physcialAddress);  
            thread->MemoryAllocated -= 0x1000;      
        }

        pageTable->UnmapMemory((void*)virtualAddressIterator);
    }
    
    return KSUCCESS;
}

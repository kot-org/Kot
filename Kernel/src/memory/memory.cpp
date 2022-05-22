#include <arch/arch.h>
#include <logs/logs.h>
#include <memory/memory.h>

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
    uint64_t NumberPage = DivideRoundUp(size, PAGE_SIZE);
    uint64_t AddressItinerator = (uint64_t)address;
    void* PagingEntry = NULL;
    __asm__ __volatile__ ("mov %%cr3, %%rax" : "=a"(PagingEntry));

    for(int i = 0; i < NumberPage; i++){
        if(!vmm_GetFlags(PagingEntry, (void*)AddressItinerator, vmm_flag::vmm_Present)){
            return false;
        } 
        if(!vmm_GetFlags(PagingEntry, (void*)AddressItinerator, vmm_flag::vmm_PhysicalStorage)){
            return false;
        } 
        AddressItinerator += PAGE_SIZE;
    }

    return true;
}


bool GetMemoryFlag(uint64_t entry, memory_share_flag flag){
    return entry & (1 << flag); 
}


/* _____________________________Share Memory_____________________________ */
//vmm_flag::vmm_Custom1 master share
//vmm_flag::vmm_Custom2 slave share

uint64_t CreatSharing(process_t* process, size_t size, uint64_t* virtualAddressPointer, uint64_t* keyPointer, uint64_t flags){
    if(CheckAddress(virtualAddressPointer, sizeof(uint64_t)) != KSUCCESS) return KFAIL;
    if(CheckAddress(keyPointer, sizeof(uint64_t)) != KSUCCESS) return KFAIL;
    void* virtualAddress = (void*)*virtualAddressPointer;
    pagetable_t pageTable = process->SharedPaging;
    uint64_t realSize = size;
    uint64_t numberOfPage = DivideRoundUp(realSize, PAGE_SIZE);
    if(GetMemoryFlag(flags, memory_share_flag_NLA)){
        if(CheckAddress(virtualAddress, realSize) != KSUCCESS) return KFAIL;
    }else{
        if((uint64_t)virtualAddress % PAGE_SIZE > 0){
            virtualAddress -= (uint64_t)virtualAddress % PAGE_SIZE;
            virtualAddress += PAGE_SIZE;
        }
        for(int i = 0; i < numberOfPage; i++){
            uint64_t virtualAddressIterator = (uint64_t)virtualAddress + i * PAGE_SIZE;
            if(!vmm_GetFlags(pageTable, (void*)virtualAddressIterator, vmm_flag::vmm_Present)){
                vmm_Map(pageTable, (void*)virtualAddressIterator, Pmm_RequestPage(), true);
                vmm_SetFlags(pageTable, (void*)virtualAddressIterator, vmm_flag::vmm_PhysicalStorage, GetMemoryFlag(flags, memory_share_flag_User)); //set master state
                process->MemoryAllocated += PAGE_SIZE;  
            }
        }        
    }

    MemoryShareInfo* shareInfo = (MemoryShareInfo*)malloc(sizeof(MemoryShareInfo));
    shareInfo->Lock = false;
    shareInfo->flags = flags;
    shareInfo->Size = realSize;
    shareInfo->PageNumber = numberOfPage;
    shareInfo->PageTableParent = pageTable;
    shareInfo->VirtualAddressParent = virtualAddress;
    shareInfo->signature0 = 'S';
    shareInfo->signature1 = 'M';

    *virtualAddressPointer = (uint64_t)virtualAddress;
    *keyPointer = (uint64_t)shareInfo;

    return KSUCCESS;
}

uint64_t GetSharing(process_t* process, MemoryShareInfo* shareInfo, uint64_t* virtualAddressPointer){
    if(CheckAddress(virtualAddressPointer, sizeof(uint64_t)) != KSUCCESS) return KFAIL;

    pagetable_t pageTable = process->SharedPaging;
    
    if(shareInfo->signature0 != 'S' || shareInfo->signature1 != 'M') return KFAIL;

    void* virtualAddress = (void*)*virtualAddressPointer;
    if(GetMemoryFlag(shareInfo->flags, memory_share_flag_NLA)){
        pagetable_t lastPageTable = vmm_GetPageTable();
        vmm_Swap(pageTable);

        uint64_t size = shareInfo->Size;
        uint64_t pages = DivideRoundUp(size, PAGE_SIZE);

        /* Allocate child memory */
        if((uint64_t)virtualAddress + pages * PAGE_SIZE < vmm_HHDMAdress){
            for(uint64_t i = 0; i < pages; i++){
                if(!vmm_GetFlags(pageTable, (void*)virtualAddress + i * PAGE_SIZE, vmm_flag::vmm_PhysicalStorage)){
                    vmm_Unmap(pageTable, (void*)virtualAddress + i * PAGE_SIZE);
                }
            }
            
            for(uint64_t i = 0; i < pages; i++){
                if(!vmm_GetFlags(pageTable, (void*)virtualAddress + i * PAGE_SIZE, vmm_flag::vmm_Present)){
                    vmm_Map(pageTable, virtualAddress + i * PAGE_SIZE, Pmm_RequestPage());
                    vmm_SetFlags(pageTable, virtualAddress + i * PAGE_SIZE, vmm_flag::vmm_PhysicalStorage, true); //set master state
                    process->MemoryAllocated += PAGE_SIZE;                    
                }
            } 
        }
        
        /* Copy memory */
        uint64_t virtualAddressParentIterator = (uint64_t)shareInfo->VirtualAddressParent;
        uint64_t virtualAddressIterator = (uint64_t)virtualAddress;
        uint64_t i = 0;
        if(virtualAddressParentIterator % PAGE_SIZE){
            uint64_t sizeToCopy = 0;
            if(size > PAGE_SIZE - (virtualAddressParentIterator % PAGE_SIZE)){
                sizeToCopy = PAGE_SIZE - (virtualAddressParentIterator % PAGE_SIZE);
            }else{
                sizeToCopy = size;
            }

            void* physicalAddressParentIterator = vmm_GetPhysical(shareInfo->PageTableParent, (void*)virtualAddressParentIterator);
            memcpy((void*)virtualAddressIterator, (void*)vmm_GetVirtualAddress(physicalAddressParentIterator), sizeToCopy);

            virtualAddressParentIterator += sizeToCopy;
            virtualAddressIterator += sizeToCopy;
            size -= sizeToCopy;
            i++;
        }
        for(; i < shareInfo->PageNumber; i++){
            uint64_t sizeToCopy;
            if(size > PAGE_SIZE){
                sizeToCopy = PAGE_SIZE;
            }else{
                sizeToCopy = size;
            }
            void* physicalAddressParentIterator = vmm_GetPhysical(shareInfo->PageTableParent, (void*)virtualAddressParentIterator);

            memcpy((void*)virtualAddressIterator, (void*)vmm_GetVirtualAddress(physicalAddressParentIterator), sizeToCopy);
            virtualAddressIterator += sizeToCopy;
            virtualAddressParentIterator += sizeToCopy;
            size -= sizeToCopy;
        } 
        vmm_Swap(lastPageTable);
    }else{
        if((uint64_t)virtualAddress % PAGE_SIZE > 0){
            virtualAddress -= (uint64_t)virtualAddress % PAGE_SIZE;
            virtualAddress += PAGE_SIZE;
        }
        for(uint64_t i = 0; i < shareInfo->PageNumber; i++){
            uint64_t virtualAddressIterator = (uint64_t)virtualAddress + i * PAGE_SIZE;
            uint64_t virtualAddressParentIterator = (uint64_t)shareInfo->VirtualAddressParent + i * PAGE_SIZE;
            void* physicalAddressParentIterator = vmm_GetPhysical(shareInfo->PageTableParent, (void*)virtualAddressParentIterator);
            vmm_Map(pageTable, (void*)virtualAddressIterator, physicalAddressParentIterator, GetMemoryFlag(shareInfo->flags, memory_share_flag_User));
            vmm_SetFlags(pageTable, (void*)virtualAddressIterator, vmm_flag::vmm_Custom2, true); //set slave state
            if(GetMemoryFlag(shareInfo->flags, memory_share_flag_ReadOnly)) vmm_SetFlags(pageTable, (void*)virtualAddressIterator, vmm_flag::vmm_ReadWrite, false); 
        }        
    }

    *virtualAddressPointer = (uint64_t)virtualAddress;
    return KSUCCESS;
}

uint64_t FreeSharing(process_t* process, MemoryShareInfo* shareInfo, void* virtualAddress){
    pagetable_t pageTable = process->SharedPaging;

    if(CheckAddress(virtualAddress, shareInfo->PageNumber * PAGE_SIZE) != KSUCCESS) return KFAIL;

    if(!GetMemoryFlag(shareInfo->flags, memory_share_flag_NLA)){
        pagetable_t pageTableMaster = shareInfo->PageTableParent;
        size_t NumberOfPage = shareInfo->PageNumber;
        for(uint64_t i = 0; i < NumberOfPage; i++){
            uint64_t virtualAddressIterator = (uint64_t)virtualAddress + i * PAGE_SIZE;
            if(vmm_GetFlags(pageTable, (void*)virtualAddressIterator, vmm_flag::vmm_PhysicalStorage)){ // is master
                void* physcialAddress = vmm_GetPhysical(pageTable, (void*)virtualAddressIterator);
                Pmm_FreePage(physcialAddress);  
                process->MemoryAllocated -= PAGE_SIZE;      
            }
            vmm_Unmap(pageTable, (void*)virtualAddressIterator);
        }
    }

    if(shareInfo->PageTableParent == pageTable){
        free((void*)shareInfo);
    }    
    
    return KSUCCESS;
}

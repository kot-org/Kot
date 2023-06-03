#include <arch/arch.h>
#include <logs/logs.h>
#include <memory/memory.h>

void memset(void* start, uint8_t value, size64_t size){
    for (uint64_t i = 0; i < size; i += sizeof(uint8_t)){
        *(uint8_t*)((uint64_t)start + i) = value;
    }
}       

void memset16(void* start, uint16_t value, size64_t size){
    for (uint64_t i = 0; i < size; i += sizeof(uint16_t)){
        *(uint16_t*)((uint64_t)start + i) = value;
    }
}

void memset32(void* start, uint32_t value, size64_t size){
    for (uint64_t i = 0; i < size; i += sizeof(uint32_t)){
        *(uint32_t*)((uint64_t)start + i) = value;
    }
}

void memset64(void* start, uint64_t value, size64_t size){
    for (uint64_t i = 0; i < size; i += sizeof(uint64_t)){
        *(uint64_t*)((uint64_t)start + i) = value;
    }
}

void memcpy(void* destination, void* source, size64_t size){
    long d0, d1, d2; 
    asm volatile(
            "rep ; movsq\n\t movq %4,%%rcx\n\t""rep ; movsb\n\t": "=&c" (d0),
            "=&D" (d1),
            "=&S" (d2): "0" (size >> 3), 
            "g" (size & 7), 
            "1" (destination),
            "2" (source): "memory"
    );  
}

int memcmp(const void *aptr, const void *bptr, size64_t size){
	const unsigned char *a = (const unsigned char*)aptr, *b = (const unsigned char*)bptr;
	for (size64_t i = 0; i < size; i++) {
		if (a[i] < b[i])
			return -1;
		else if (a[i] > b[i])
			return 1;
	}
	return 0;
}

bool CheckAddress(void* address, size64_t size, void* pagingEntry){
    if(address == NULL) return false;
    uint64_t NumberPage = DivideRoundUp(size, PAGE_SIZE);
    uint64_t AddressItinerator = (uint64_t)address;

    for(uint64_t i = 0; i < NumberPage; i++){
        if(!vmm_GetFlags(pagingEntry, (void*)AddressItinerator, vmm_flag::vmm_Present)){
            return false;
        } 
        AddressItinerator += PAGE_SIZE;
    }

    return true;
}

bool CheckAddress(void* address, size64_t size){
    void* PagingEntry = NULL;
    __asm__ __volatile__ ("mov %%cr3, %%rax" : "=a"(PagingEntry));
    return CheckAddress(address, size, PagingEntry);
}

bool CheckUserAddress(void* address, size64_t size, void* pagingEntry){
    if(address == NULL) return false;
    if((uint64_t)address >= VMM_HIGHER_HALF_ADDRESS){
        return false;
    }
    uint64_t NumberPage = DivideRoundUp(size, PAGE_SIZE);
    uint64_t AddressItinerator = (uint64_t)address;

    for(uint64_t i = 0; i < NumberPage; i++){
        if(!vmm_GetFlags(pagingEntry, (void*)AddressItinerator, vmm_flag::vmm_Present)){
            return false;
        } 
        AddressItinerator += PAGE_SIZE;
    }

    return true;
}

bool CheckUserAddress(void* address, size64_t size){
    void* PagingEntry = NULL;
    __asm__ __volatile__ ("mov %%cr3, %%rax" : "=a"(PagingEntry));
    return CheckAddress(address, size, PagingEntry);
}


/* _____________________________Share Memory_____________________________ */
//vmm_flag::vmm_Custom1 master share
//vmm_flag::vmm_Slave slave share

uint64_t CreateMemoryField(kthread_t* self, kprocess_t* process, size64_t size, uint64_t* virtualAddressPointer, uint64_t* keyPointer, enum MemoryFieldType type){
    if(CheckAddress(virtualAddressPointer, sizeof(uint64_t)) != KSUCCESS) return KFAIL;
    if(CheckAddress(keyPointer, sizeof(uint64_t)) != KSUCCESS) return KFAIL;
    void* virtualAddress = (void*)*virtualAddressPointer;
    uint64_t offset = NULL;
    pagetable_t pageTable = process->SharedPaging;
    uint64_t realSize = size;
    uint64_t numberOfPage = DivideRoundUp(realSize, PAGE_SIZE);
    switch(type){
        case MemoryFieldTypeShareSpaceRW:{
            offset = ((uint64_t)virtualAddress) % PAGE_SIZE;
            if((uint64_t)virtualAddress % PAGE_SIZE > 0){
                virtualAddress = (void*)((uint64_t)virtualAddress - (uint64_t)virtualAddress % PAGE_SIZE);
            }
            for(uint64_t i = 0; i < numberOfPage; i++){
                uint64_t virtualAddressIterator = (uint64_t)virtualAddress + i * PAGE_SIZE;
                if(!vmm_GetFlags(pageTable, (void*)virtualAddressIterator, vmm_flag::vmm_Present)){
                    vmm_Map(pageTable, (void*)virtualAddressIterator, Pmm_RequestPage(), true, true, false);
                    vmm_SetFlags(pageTable, (void*)virtualAddressIterator, vmm_flag::vmm_Master, true); //set master state
                    process->MemoryAllocated += PAGE_SIZE;  
                }
            }  
            break;
        }
        case MemoryFieldTypeShareSpaceRO:{
            offset = ((uint64_t)virtualAddress) % PAGE_SIZE;
            if((uint64_t)virtualAddress % PAGE_SIZE > 0){
                virtualAddress = (void*)((uint64_t)virtualAddress - (uint64_t)virtualAddress % PAGE_SIZE);
            }
            for(uint64_t i = 0; i < numberOfPage; i++){
                uint64_t virtualAddressIterator = (uint64_t)virtualAddress + i * PAGE_SIZE;
                if(!vmm_GetFlags(pageTable, (void*)virtualAddressIterator, vmm_flag::vmm_Present)){
                    vmm_Map(pageTable, (void*)virtualAddressIterator, Pmm_RequestPage(), true, true, false); // the master can write into memory even if it's read only for slave
                    vmm_SetFlags(pageTable, (void*)virtualAddressIterator, vmm_flag::vmm_Master, true); //set master state
                    process->MemoryAllocated += PAGE_SIZE;  
                }
            }  
            break;
        }
        case MemoryFieldTypeSendSpaceRO:{
            pagetable_t lastPageTable = vmm_GetPageTable();
            if(CheckAddress(virtualAddress, realSize, pageTable) != KSUCCESS) return KFAIL;
            if(lastPageTable != pageTable) vmm_Swap(self, pageTable);
            if((uint64_t)virtualAddress % PAGE_SIZE){
                size64_t nonAlignedSize = PAGE_SIZE - ((uint64_t)virtualAddress % PAGE_SIZE);
                if(realSize > nonAlignedSize){
                    numberOfPage++;
                }
            }
            if(lastPageTable != pageTable) vmm_Swap(self, lastPageTable);
            break;
        }
    }

    MemoryShareInfo* shareInfo = (MemoryShareInfo*)kmalloc(sizeof(MemoryShareInfo));
    AtomicClearLock(&shareInfo->Lock);
    shareInfo->InitialSize = size;
    shareInfo->Type = type;
    shareInfo->RealSize = realSize;
    shareInfo->PageNumber = numberOfPage;
    shareInfo->Parent = process;
    shareInfo->PageTableParent = pageTable;
    shareInfo->VirtualAddressParent = virtualAddress;
    shareInfo->SlavesList = KStackInitialize(0x50);
    shareInfo->SlavesNumber = NULL;
    shareInfo->Offset = offset;
    shareInfo->signature0 = 'S';
    shareInfo->signature1 = 'M';

    *virtualAddressPointer = (uint64_t)virtualAddress;
    *keyPointer = (uint64_t)shareInfo;

    return KSUCCESS;
}

uint64_t AcceptMemoryField(kthread_t* self, kprocess_t* process, MemoryShareInfo* shareInfo, uint64_t* virtualAddressPointer){
    pagetable_t pageTable = process->SharedPaging;

    if(shareInfo->signature0 != 'S' || shareInfo->signature1 != 'M') return KFAIL;

    AtomicAquire(&shareInfo->Lock);

    void* virtualAddress = (void*)*virtualAddressPointer;
    
    switch(shareInfo->Type){
        case MemoryFieldTypeShareSpaceRW:{
            if((uint64_t)virtualAddress % PAGE_SIZE > 0){
                virtualAddress = (void*)((uint64_t)virtualAddress - (uint64_t)virtualAddress % PAGE_SIZE);
            }
            for(uint64_t i = 0; i < shareInfo->PageNumber; i++){
                uint64_t virtualAddressIterator = (uint64_t)virtualAddress + i * PAGE_SIZE;
                uint64_t virtualAddressParentIterator = (uint64_t)shareInfo->VirtualAddressParent + i * PAGE_SIZE;
                void* physicalAddressParentIterator = vmm_GetPhysical(shareInfo->PageTableParent, (void*)virtualAddressParentIterator);
                vmm_Map(pageTable, (void*)virtualAddressIterator, physicalAddressParentIterator, true, true, false);
                vmm_SetFlags(pageTable, (void*)virtualAddressIterator, vmm_flag::vmm_Slave, true); //set slave state
            }
            break;
        }
        case MemoryFieldTypeShareSpaceRO:{
            if((uint64_t)virtualAddress % PAGE_SIZE > 0){
                virtualAddress = (void*)((uint64_t)virtualAddress - (uint64_t)virtualAddress % PAGE_SIZE);
            }
            for(uint64_t i = 0; i < shareInfo->PageNumber; i++){
                uint64_t virtualAddressIterator = (uint64_t)virtualAddress + i * PAGE_SIZE;
                uint64_t virtualAddressParentIterator = (uint64_t)shareInfo->VirtualAddressParent + i * PAGE_SIZE;
                void* physicalAddressParentIterator = vmm_GetPhysical(shareInfo->PageTableParent, (void*)virtualAddressParentIterator);
                vmm_Map(pageTable, (void*)virtualAddressIterator, physicalAddressParentIterator, true, false, false);
                vmm_SetFlags(pageTable, (void*)virtualAddressIterator, vmm_flag::vmm_Slave, true); //set slave state
            }            
            break;
        }
        case MemoryFieldTypeSendSpaceRO:{
            pagetable_t lastPageTable = vmm_GetPageTable();
            vmm_Swap(self, pageTable);

            uint64_t alignement = (uint64_t)virtualAddress & 0xFFF;
            uint64_t size = shareInfo->RealSize;
            uint64_t pages = DivideRoundUp(size + alignement, PAGE_SIZE);

            /* Allocate child memory */
            if((uint64_t)virtualAddress + pages * PAGE_SIZE < vmm_HHDMAdress){
                for(uint64_t i = 0; i < pages; i++){
                    if(!vmm_GetFlags(pageTable, (void*)((uint64_t)virtualAddress + i * PAGE_SIZE), vmm_flag::vmm_Present)){
                        vmm_Unmap(pageTable, (void*)((uint64_t)virtualAddress + i * PAGE_SIZE));
                    }
                }
                
                for(uint64_t i = 0; i < pages; i++){
                    if(!vmm_GetFlags(pageTable, (void*)((uint64_t)virtualAddress + i * PAGE_SIZE), vmm_flag::vmm_Present)){
                        vmm_Map(pageTable, (void*)((uint64_t)virtualAddress + i * PAGE_SIZE), Pmm_RequestPage(), true, true, false);
                        vmm_SetFlags(pageTable, (void*)((uint64_t)virtualAddress + i * PAGE_SIZE), vmm_flag::vmm_Master, true); //set master state
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

            while(size != 0){
                uint64_t sizeToCopy = size;
                if(size > PAGE_SIZE){
                    sizeToCopy = PAGE_SIZE;
                }

                void* physicalAddressParentIterator = vmm_GetPhysical(shareInfo->PageTableParent, (void*)virtualAddressParentIterator);

                memcpy((void*)virtualAddressIterator, (void*)vmm_GetVirtualAddress(physicalAddressParentIterator), sizeToCopy);
                virtualAddressIterator += sizeToCopy;
                virtualAddressParentIterator += sizeToCopy;
                size -= sizeToCopy;
            } 
            vmm_Swap(self, lastPageTable);
            break;
        }
    }

    SlaveInfo_t* SlaveInfo = (SlaveInfo_t*)kmalloc(sizeof(SlaveInfo_t));
    SlaveInfo->process = process;
    SlaveInfo->virtualAddress = virtualAddress;
    shareInfo->SlavesList->push64((uint64_t)SlaveInfo);
    shareInfo->SlavesNumber++;

    AtomicRelease(&shareInfo->Lock);
    
    *virtualAddressPointer = (uint64_t)virtualAddress + shareInfo->Offset;
    return KSUCCESS;
}

uint64_t CloseMemoryField(kthread_t* self, kprocess_t* process, MemoryShareInfo* shareInfo, void* virtualAddress){
    pagetable_t pageTable = process->SharedPaging;
    bool IsParent = (process == shareInfo->Parent);

    if(CheckAddress(virtualAddress, shareInfo->PageNumber * PAGE_SIZE) != KSUCCESS) return KFAIL;

    if(IsParent){
        for(uint64_t i = 0; i < shareInfo->SlavesNumber; i++){
            SlaveInfo_t* SlaveInfo = (SlaveInfo_t*)shareInfo->SlavesList->pop64();
            CloseMemoryField(self, SlaveInfo->process, shareInfo, SlaveInfo->virtualAddress);
        }        
    }

    AtomicAquire(&shareInfo->Lock);

    switch(shareInfo->Type){
        case MemoryFieldTypeShareSpaceRW:{
            pagetable_t pageTableMaster = shareInfo->PageTableParent;
            size64_t NumberOfPage = shareInfo->PageNumber;
            for(uint64_t i = 0; i < NumberOfPage; i++){
                uint64_t virtualAddressIterator = (uint64_t)virtualAddress + i * PAGE_SIZE;
                if(vmm_GetFlags(pageTable, (void*)virtualAddressIterator, vmm_flag::vmm_Master)){ // is master
                    void* physcialAddress = vmm_GetPhysical(pageTable, (void*)virtualAddressIterator);
                    Pmm_FreePage(physcialAddress);  
                    process->MemoryAllocated -= PAGE_SIZE;      
                }
                vmm_Unmap(pageTable, (void*)virtualAddressIterator);
            }
            break;
        }
        case MemoryFieldTypeShareSpaceRO:{
            pagetable_t pageTableMaster = shareInfo->PageTableParent;
            size64_t NumberOfPage = shareInfo->PageNumber;
            for(uint64_t i = 0; i < NumberOfPage; i++){
                uint64_t virtualAddressIterator = (uint64_t)virtualAddress + i * PAGE_SIZE;
                if(vmm_GetFlags(pageTable, (void*)virtualAddressIterator, vmm_flag::vmm_Master)){ // is master
                    void* physcialAddress = vmm_GetPhysical(pageTable, (void*)virtualAddressIterator);
                    Pmm_FreePage(physcialAddress);  
                    process->MemoryAllocated -= PAGE_SIZE;      
                }
                vmm_Unmap(pageTable, (void*)virtualAddressIterator);
            }
            break;
        }
        case MemoryFieldTypeSendSpaceRO:{
            break;
        }
    }
    
    if(IsParent){
        kfree((void*)shareInfo);
    }
    AtomicRelease(&shareInfo->Lock);
    
    return KSUCCESS;
}

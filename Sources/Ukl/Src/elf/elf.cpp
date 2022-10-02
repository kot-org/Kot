#include <elf/elf.h>

KResult loadElf(uintptr_t buffer, uint64_t* entrypoint){
    Elf64_Ehdr* Header = (Elf64_Ehdr*)buffer;
    
    /* Check elf */
    if(Header->e_ident[0] != EI_MAG0 || Header->e_ident[1] != EI_MAG1 || Header->e_ident[2] != EI_MAG2 || Header->e_ident[3] != EI_MAG3){
        return KFAIL;
    }
    
    /* Load the elf */
    uint64_t phdrs = (uint64_t)((uint64_t)buffer + Header->e_phoff);
    uint64_t shdrs = (uint64_t)((uint64_t)buffer + Header->e_shoff);

    for(Elf64_Half i = 0; i < Header->e_phnum; i++){
        Elf64_Phdr* phdr = (Elf64_Phdr*)((uint64_t)phdrs + (i * Header->e_phentsize));
        if(phdr->p_type == PT_LOAD){
            size64_t align = phdr->p_vaddr & 0xFFF; // get last 9 bits
            size64_t pageCount = DivideRoundUp(phdr->p_memsz + align, PAGE_SIZE);
            size_t size = phdr->p_memsz;
            uint64_t totalSizeCopy = NULL;

            uint64_t virtualAddressParentIterator = (uint64_t)buffer + phdr->p_offset;
            uint64_t virtualAddressIterator = (uint64_t)phdr->p_vaddr;
            uint64_t i = 0;
            if(virtualAddressIterator % PAGE_SIZE){
                uint64_t sizeToCopy = 0;
                uint64_t alignement = virtualAddressIterator % PAGE_SIZE;
                if(size > PAGE_SIZE - alignement){
                    sizeToCopy = PAGE_SIZE - alignement;
                }else{
                    sizeToCopy = size;
                }

                uint64_t physicalPage = NULL;
                if(!vmm_GetFlags(vmm_PageTable, virtualAddressIterator, vmm_PhysicalStorage)){
                    physicalPage = Pmm_RequestPage();
                    vmm_Map(vmm_PageTable, ((uint64_t)virtualAddressIterator), physicalPage, false, true, true);
                    physicalPage = ((uint64_t)physicalPage + alignement);
                }else{
                    physicalPage = vmm_GetPhysical(vmm_PageTable, virtualAddressIterator);
                }
                if(totalSizeCopy < phdr->p_filesz){
                    memcpy((uintptr_t)(vmm_GetVirtualAddress(physicalPage)), (uintptr_t)virtualAddressParentIterator, sizeToCopy);
                }else{
                    memset((uintptr_t)(vmm_GetVirtualAddress(physicalPage)), NULL, sizeToCopy);
                }

                virtualAddressParentIterator += sizeToCopy;
                virtualAddressIterator += sizeToCopy;
                totalSizeCopy += sizeToCopy;
                size -= sizeToCopy;
                i++;
            }
            for(; i < pageCount; i++){
                uint64_t sizeToCopy;
                if(size > PAGE_SIZE){
                    sizeToCopy = PAGE_SIZE;
                }else{
                    sizeToCopy = size;
                }
            
                uint64_t physicalPage = NULL;
                if(!vmm_GetFlags(vmm_PageTable, virtualAddressIterator, vmm_PhysicalStorage)){
                    physicalPage = Pmm_RequestPage();
                    vmm_Map(vmm_PageTable, (uint64_t)virtualAddressIterator, (uint64_t)physicalPage, false, true, true);
                }else{
                    physicalPage = vmm_GetPhysical(vmm_PageTable, virtualAddressIterator);
                }
                if(totalSizeCopy < phdr->p_filesz){
                    memcpy((uintptr_t)(vmm_GetVirtualAddress(physicalPage)), (uintptr_t)virtualAddressParentIterator, sizeToCopy);
                }else{
                    memset((uintptr_t)(vmm_GetVirtualAddress(physicalPage)), NULL, sizeToCopy);
                }

                virtualAddressIterator += sizeToCopy;
                virtualAddressParentIterator += sizeToCopy;
                totalSizeCopy += sizeToCopy;
                size -= sizeToCopy;
            }
        }
    }

    *entrypoint = Header->e_entry;
    return KSUCCESS;
}
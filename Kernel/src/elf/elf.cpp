#include <elf/elf.h>

namespace ELF{
    KResult loadElf(void* buffer, uint8_t ring, Parameters* FunctionParameters){
        elf_t* self = (elf_t*)calloc(sizeof(elf_t));
        self->Buffer = buffer;
        self->Header = (Elf64_Ehdr*)buffer;
        
        /* Check elf */
        if(Check(self)){
            free(self);
            return KFAIL;
        }
        
        process_t* proc = NULL;
        globalTaskManager->CreatProcess(&proc, ring, 0);
        thread_t* mainThread = proc->CreatThread((void*)self->Header->e_entry, NULL);
        
        /* Load the elf */
        void* phdrs = (void*)(uint64_t)buffer + self->Header->e_phoff;

        for(int i = 0; i < self->Header->e_phnum; i++){
            Elf64_Phdr* phdr = (Elf64_Phdr*)((uint64_t)phdrs + (i * self->Header->e_phentsize));
            if(phdr->p_type == PT_LOAD){
                size_t align = phdr->p_vaddr & (PAGE_SIZE - 1); // get last 9 bits
                size_t pageCount = DivideRoundUp(phdr->p_memsz + align, PAGE_SIZE);

                
                for(uint64_t y = 0; y < pageCount * PAGE_SIZE; y += PAGE_SIZE){
                    void* DirectAddressToCopy = 0;
                    void* VirtualAddress = (void*)(phdr->p_vaddr + y);
                    void* PhysicalBuffer = 0;
                    if(!vmm_GetFlags(mainThread->Paging, VirtualAddress, vmm_flag::vmm_Present)){
                        PhysicalBuffer = Pmm_RequestPage();
                        vmm_Map(mainThread->Paging, VirtualAddress, (void*)PhysicalBuffer, true);
                        DirectAddressToCopy = (void*)(vmm_GetVirtualAddress(PhysicalBuffer) + (uint64_t)align);
                    }else{
                        DirectAddressToCopy = (void*)(vmm_GetVirtualAddress(vmm_GetPhysical(mainThread->Paging, VirtualAddress)) + (uint64_t)align);
                    }
                    if(y < phdr->p_filesz){
                        size_t SizeToCopy = 0;
                        if(phdr->p_filesz - y > PAGE_SIZE){
                            SizeToCopy = PAGE_SIZE;
                        }else{
                            SizeToCopy = phdr->p_filesz - y;
                        }
                        memcpy((void*)DirectAddressToCopy, (void*)((uint64_t)buffer + phdr->p_offset + y), SizeToCopy);  
                    }else{
                        size_t SizeToFill = 0;
                        if(phdr->p_memsz - y > PAGE_SIZE){
                            SizeToFill = PAGE_SIZE;
                        }else{
                            SizeToFill = phdr->p_memsz - y;
                        }
                        memcpy((void*)DirectAddressToCopy, (void*)((uint64_t)buffer + phdr->p_offset + y), SizeToFill);  
                    }
                    
                }
            }
        }

        mainThread->Launch(FunctionParameters);
        free(self);
        return KSUCCESS;
    }

    bool Check(elf_t* self){
        return (self->Header->e_ident[0] != EI_MAG0 || self->Header->e_ident[1] != EI_MAG1 || self->Header->e_ident[2] != EI_MAG2 || self->Header->e_ident[3] != EI_MAG3);
    }
}

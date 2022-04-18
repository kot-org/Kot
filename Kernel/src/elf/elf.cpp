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
        thread_t* mainThread = proc->CreatThread((uint64_t)self->Header->e_entry, NULL);
        
        /* Load the elf */
        LoadBinary(mainThread->Paging, self, 0);
        
        mainThread->Launch(FunctionParameters);
        return KSUCCESS;
    }

    bool Check(elf_t* self){
        return (self->Header->e_ident[0] != EI_MAG0 || self->Header->e_ident[1] != EI_MAG1 || self->Header->e_ident[2] != EI_MAG2 || self->Header->e_ident[3] != EI_MAG3);
    }

    void LoadBinary(pagetable_t table, struct elf_t* self, uint64_t address){
        void* phdrs = (void*)((uint64_t)self->Buffer + (uint64_t)self->Header->e_phoff);
        for(int i = 0; i < self->Header->e_phnum; i++){
            Elf64_Phdr* phdr = (Elf64_Phdr*)((uint64_t)phdrs + (i * self->Header->e_phentsize));
            switch (phdr->p_type){
                case PT_LOAD:
                {	
                    Elf64_Addr segment = phdr->p_vaddr + address;

                    uint64_t pages = Divide(phdr->p_memsz, PAGE_SIZE);
                    uint64_t size = phdr->p_filesz;
                    for(uint64_t y = 0; y < pages; y++){
                        uint64_t SizeToCopy = 0;
                        if(size > PAGE_SIZE){
                            SizeToCopy = PAGE_SIZE; 
                        }else{
                            SizeToCopy = size;
                        }
                        size -= SizeToCopy; 
                        void* virtualAddress = (void*)(segment + y * PAGE_SIZE);
                        uint64_t offset = (uint64_t)virtualAddress % 0x1000;
                        virtualAddress -= offset;
                        //Custom 0 flags : is user executable
                        if(!vmm_GetFlags(table, virtualAddress, vmm_flag::vmm_Custom0)){
                            void* PhysicalBuffer = Pmm_RequestPage();
                            vmm_Map(table, (void*)virtualAddress, (void*)PhysicalBuffer, true);
                            vmm_SetFlags(table, virtualAddress, vmm_flag::vmm_Custom0, true);
                            memcpy((void*)vmm_GetVirtualAddress(PhysicalBuffer) + offset, (void*)((uint64_t)self->Buffer + phdr->p_offset), SizeToCopy);
                        }
                    }
                    break;   
                }
            }
        }
    }
}

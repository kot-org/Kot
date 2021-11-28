#include "elf.h"

namespace ELF{
    int loadElf(void* buffer, int ring, char* name){
        Elf64_Ehdr* header = (Elf64_Ehdr*)buffer;
        //check elf
        if(header->e_ident[0] != EI_MAG0 || header->e_ident[1] != EI_MAG1 || header->e_ident[2] != EI_MAG2 || header->e_ident[3] != EI_MAG3) return 0;
        TaskNode* task = globalTaskManager->AddTask(false, true, ring, name);
        globalPageTableManager.ChangePaging(&task->Content.paging);

        //Get location data
        void* phdrs = (void*)(uint64_t)buffer + header->e_phoff;

        for(int i = 0; i < header->e_phnum; i++){
            Elf64_Phdr* phdr = (Elf64_Phdr*)((uint64_t)phdrs + (i * header->e_phentsize));
            Elf64_Addr segment = phdr->p_paddr;

            int pages = Divide(phdr->p_memsz, 0x1000);
            for(uint64_t y = 0; y < pages; y++){
                void* virtualAddress = (void*)(segment + y * 0x1000);
                if(!task->Content.paging.GetFlags(virtualAddress, PT_Flag::IsUserExecutable)){
                    void* PhysicalBuffer = globalAllocator.RequestPage();
                    task->Content.paging.MapMemory((void*)virtualAddress, (void*)PhysicalBuffer);
                    task->Content.paging.MapUserspaceMemory((void*)virtualAddress);
                    task->Content.paging.SetFlags(virtualAddress, PT_Flag::IsUserExecutable, true);
                }
            }
            memcpy((void*)segment, (void*)((uint64_t)buffer + phdr->p_offset), phdr->p_filesz);   
        }
        globalPageTableManager.RestorePaging();
        task->Content.Launch((void*)header->e_entry);
        return 1;
    }    
}


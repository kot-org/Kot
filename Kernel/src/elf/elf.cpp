#include "elf.h"

namespace ELF{
    int loadElf(void* buffer){
        Elf64_Ehdr* header = (Elf64_Ehdr*)buffer;
        //check elf
        if(header->e_ident[0] != EI_MAG0 || header->e_ident[1] != EI_MAG1 || header->e_ident[2] != EI_MAG2 || header->e_ident[3] != EI_MAG3) return 0;
        TaskNode* task = globalTaskManager.AddTask(false, true, 3);
        globalPageTableManager.ChangePaging(&task->Content.paging);

        //Get location data
        void* phdrs = (void*)(uint64_t)buffer + header->e_phoff;

        for(int i = 0; i < header->e_phnum; i++){
            Elf64_Phdr* phdr = (Elf64_Phdr*)((uint64_t)phdrs + (i * header->e_phentsize));
            Elf64_Addr segment = phdr->p_paddr;

            int pages = Divide(phdr->p_memsz, 0x1000);
            for(uint64_t y = 0; y < pages; y++){
                void* PhysicalBuffer = globalAllocator.RequestPage();
                task->Content.paging.MapMemory((void*)segment + i * 0x1000, (void*)PhysicalBuffer);
                task->Content.paging.MapUserspaceMemory((void*)segment + i * 0x1000);
            }	
            size_t size = phdr->p_filesz;
            memcpy((void*)segment, (void*)((uint64_t)buffer + phdr->p_offset), size);            
        }
        
        globalPageTableManager.RestorePaging();
        task->Content.Launch((void*)header->e_entry);
        return 1;
    }    
}


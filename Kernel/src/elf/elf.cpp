#include "elf.h"

namespace ELF{
    int loadElf(void* buffer, uint8_t ring, Parameters* FunctionParameters){
        Elf64_Ehdr* header = (Elf64_Ehdr*)buffer;
        //check elf
        if(header->e_ident[0] != EI_MAG0 || header->e_ident[1] != EI_MAG1 || header->e_ident[2] != EI_MAG2 || header->e_ident[3] != EI_MAG3) return 0;
        process_t* proc = globalTaskManager->CreatProcess(ring, 0);
        thread_t* mainThread = proc->CreatThread(header->e_entry, NULL);
        globalPageTableManager[0].ChangePaging(mainThread->Paging);

        //Get location data
        void* phdrs = (void*)(uint64_t)buffer + header->e_phoff;

        for(int i = 0; i < header->e_phnum; i++){
            Elf64_Phdr* phdr = (Elf64_Phdr*)((uint64_t)phdrs + (i * header->e_phentsize));
            Elf64_Addr segment = phdr->p_paddr;

            int pages = Divide(phdr->p_memsz, 0x1000);
            for(uint64_t y = 0; y < pages; y++){
                void* virtualAddress = (void*)(segment + y * 0x1000);
                //Custom 0 flags : is user executable
                if(!mainThread->Paging->GetFlags(virtualAddress, PT_Flag::Custom0)){
                    void* PhysicalBuffer = globalAllocator.RequestPage();
                    mainThread->Paging->MapMemory((void*)virtualAddress, (void*)PhysicalBuffer);
                    mainThread->Paging->MapUserspaceMemory((void*)virtualAddress);
                    mainThread->Paging->SetFlags(virtualAddress, PT_Flag::Custom0, true);
                }
            }
            memcpy((void*)segment, (void*)((uint64_t)buffer + phdr->p_offset), phdr->p_filesz);   
        }

        globalPageTableManager[0].RestorePaging();
        mainThread->Launch(FunctionParameters);
        return 1;
    }    
}


#include <elf/elf.h>

namespace ELF{
    int loadElf(void* buffer, uint8_t ring, Parameters* FunctionParameters){
        Elf64_Ehdr* header = (Elf64_Ehdr*)buffer;
        //check elf
        if(header->e_ident[0] != EI_MAG0 || header->e_ident[1] != EI_MAG1 || header->e_ident[2] != EI_MAG2 || header->e_ident[3] != EI_MAG3) return 0;
        
        process_t* proc = NULL;
        globalTaskManager->CreatProcess(&proc, ring, 0);
        thread_t* mainThread = proc->CreatThread((uint64_t)header->e_entry, NULL);
        globalPageTableManager[0].ChangePaging(mainThread->Paging);

        /* Get location data */
        void* phdrs = (void*)((uint64_t)buffer + (uint64_t)header->e_phoff);

        Elf64_Shdr* shdr = (Elf64_Shdr*)((uint64_t)buffer + (uint64_t)header->e_shoff);
        Elf64_Shdr* strt = (Elf64_Shdr*)((uint64_t)shdr + ((uint64_t)header->e_shstrndx * (uint64_t)header->e_shentsize));

        char* strtable = (char*)((uint64_t)buffer + strt->sh_offset);

        Elf64_Shdr* RelaSH = NULL;
        Elf64_Shdr* RelatSH = NULL;
        Elf64_Shdr* DynSH = NULL;
        Elf64_Shdr* GotSH = NULL;
        Elf64_Shdr* DynsymSH = NULL;
        Elf64_Shdr* DynstrSH = NULL;
        Elf64_Shdr* SymSH = NULL;
        Elf64_Shdr* StrSH = NULL;


        /* Read headers */
        for(int i = 0; i < header->e_phnum; i++){
            Elf64_Phdr* phdr = (Elf64_Phdr*)((uint64_t)phdrs + (i * header->e_phentsize));
            switch (phdr->p_type){
                case PT_LOAD:
                {	
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
                    break;   
                }
                case PT_DYNAMIC:
                {
                    uint64_t occurence = phdr->p_memsz / sizeof(Elf64_Dyn);
                    Elf64_Dyn* dyn = (Elf64_Dyn*)(buffer + phdr->p_offset);
                    for(uint64_t i = 0; i < occurence; i++){
                        switch(dyn->d_tag) {
                            case DT_STRTAB:
                            {
                                char* strtable;
                                strtable = (char*)buffer + dyn->d_un.d_ptr; 
                                break;
                            } 
                            case DT_STRSZ:
                            {
                                int strsz;
                                strsz = dyn->d_un.d_val; 
                                break;
                            } 
                            case DT_SYMTAB:
                            {
                                Elf64_Sym* sym = NULL;
                                sym = (Elf64_Sym*)(buffer + dyn->d_un.d_ptr); 
                                break;
                            } 
                            case DT_SYMENT:
                            {
                                int syment;
                                syment = dyn->d_un.d_val; 
                                break;
                            } 
                        }
                        dyn++;

                    }
                    break;
                }
            }
        }

        /* Read sections */

        for(uint64_t i = 0; i < header->e_shnum; i++){
            char* name = (char*)((uint64_t)strtable + (uint64_t)shdr->sh_name);
            if(strcmp(name, ".rela.plt")){
                RelaSH = shdr;
            }
            if(strcmp(name, ".rela.text") || strcmp(name, ".rela.dyn")){
                RelatSH = shdr;
            }
            if(strcmp(name, ".dynamic")){
                DynSH = shdr;
            }
            if(strcmp(name, ".got.plt")){
                GotSH = shdr;
            }
            if(strcmp(name, ".dynsym")){
                DynsymSH = shdr;
            }
            if(strcmp(name, ".symtab")){
                SymSH = shdr;
            }
            if(strcmp(name, ".dynstr")){
                DynstrSH = shdr;
            }
            if(strcmp(name, ".strtab")){
                StrSH = shdr;
            }

            shdr = (Elf64_Shdr*)((uint8_t*)shdr + header->e_shentsize);
        }


        /* Get libraries */

        if(DynSH != NULL){
            Elf64_Dyn* dyn = (Elf64_Dyn*)((uint64_t)buffer + DynSH->sh_offset);
            uint64_t num = DynSH->sh_size / DynSH->sh_entsize;
            
            for(uint64_t i = 0; i < num; i++){
                if(dyn->d_tag == DT_NEEDED){
                    char* name = (char*)((uint64_t)buffer + (uint64_t)DynstrSH->sh_offset + (uint64_t)dyn->d_un.d_ptr);
                    globalLogs->Successful("%s", name);
                }

                dyn = (Elf64_Dyn*)((uint8_t*)dyn + DynSH->sh_entsize);
            }
        }
        
        globalPageTableManager[0].RestorePaging();
        mainThread->Launch(FunctionParameters);
        return 1;
    }    
}
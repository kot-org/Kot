#include <elf/elf.h>

namespace ELF {

    static inline char* GetSectionName(elf_t* self, Elf64_Shdr* shdr) {
        return (char*)((uint64_t)self->Buffer + self->shstr->sh_offset + shdr->sh_name);
    }
 
    static inline Elf64_Shdr* GetSectionHeaderIndex(elf_t* self, Elf64_Half index) {
        return (Elf64_Shdr*)((uint64_t)self->shdrs + (index * self->Header->e_shentsize));
    }

    static inline Elf64_Shdr* GetSectionHeaderName(elf_t* self, char* name) {
        for(Elf64_Half i = 0; i <  self->Header->e_shnum; i++){
            Elf64_Shdr* shdr = GetSectionHeaderIndex(self, i);
            if(!strcmp(GetSectionName(self, shdr), name)){
                return shdr;
            }
        }
        return NULL;
    }

    static inline Elf64_Shdr* GetSectionHeaderType(elf_t* self, uint32_t type) {
        for(Elf64_Half i = 0; i < self->Header->e_shnum; i++){
            Elf64_Shdr* shdr = GetSectionHeaderIndex(self, i);
            if(shdr->sh_type == type){
                return shdr;
            }
        }
        return NULL;
    }

    static inline Elf64_Half GetSectionIndex(elf_t* self, Elf64_Shdr* shdr) {
        return (Elf64_Half)(((uint64_t)shdr - (uint64_t)self->shdrs) / self->Header->e_shentsize);
    }

    KResult loadElf(void* buffer, kot_process_t proc, enum kot_Priviledge privilege, uint64_t identifier, kot_thread_t* mainthread, char* rootpath, bool isVFS){
        elf_t* self = (elf_t*)calloc(1, sizeof(elf_t));
        self->Buffer = buffer;
        self->Header = (Elf64_Ehdr*)buffer;

        /* Check elf */
        if(Check(self)){
            free(self);
            return KFAIL;
        }

        
        if(!proc){
            if(kot_Sys_CreateProc(&proc, privilege, identifier) != KSUCCESS) return KFAIL;
        }
        /* TODO : create thread identifier */
        if(kot_Sys_CreateThread(proc, (void*)self->Header->e_entry, privilege, NULL, mainthread) != KSUCCESS) return KFAIL;

        kot_process_t parentProcess = kot_Sys_GetProcess();

        /* Load the elf */
        self->phdrs = (void*)((uint64_t)buffer + self->Header->e_phoff);
        self->shdrs = (void*)((uint64_t)buffer + self->Header->e_shoff);
        
        self->shstr = GetSectionHeaderIndex(self, self->Header->e_shstrndx);
        self->KotSpecific = GetSectionHeaderName(self, ".KotSpecificData");

        kot_thread_t Runningthread = kot_Sys_GetThread();

        uint64_t HeapLocation = 0x0;
        for(uint64_t i = 0; i < self->Header->e_phnum; i++){
            Elf64_Phdr* phdr = (Elf64_Phdr*)((uint64_t)self->phdrs + (i * self->Header->e_phentsize));
            if((phdr->p_vaddr + phdr->p_memsz) > HeapLocation){
                HeapLocation = phdr->p_vaddr + phdr->p_memsz;
            }
        }
        kot_SpecificData_t* KotSpecificDataClient = NULL;
        if(self->KotSpecific != NULL){
            /* Check symbol size */
            if(self->KotSpecific->sh_size >= sizeof(kot_SpecificData_t)){
                if(self->KotSpecific->sh_size != sizeof(kot_SpecificData_t)){
                    kot_Printlog("WARNING : This ELF might be corrupter OR doesn't respect KOT specifications");
                }
                if(HeapLocation % KotSpecificData.MMapPageSize){
                    HeapLocation -= HeapLocation % KotSpecificData.MMapPageSize;
                    HeapLocation += KotSpecificData.MMapPageSize;
                }
                KotSpecificDataClient = (kot_SpecificData_t*)calloc(1, sizeof(kot_SpecificData_t));
                KotSpecificDataClient->MMapPageSize = KotSpecificData.MMapPageSize;
                KotSpecificDataClient->HeapLocation = HeapLocation;
                KotSpecificDataClient->FreeMemorySpace = KotSpecificData.FreeMemorySpace;
                KotSpecificDataClient->UISDHandler = KotSpecificData.UISDHandler;

                if(isVFS){
                    kot_srv_storage_callback_t* Callback = kot_Srv_Storage_VFSLoginApp(kot_ShareProcessKey(proc), FS_AUTHORIZATION_HIGH, Storage_Permissions_Admin | Storage_Permissions_Read | Storage_Permissions_Write | Storage_Permissions_Create, rootpath, true);
                    KotSpecificDataClient->VFSHandler = Callback->Data;
                    free(Callback);
                }
                void* DataAddress = (void*)(self->KotSpecific->sh_offset + (uint64_t)self->Buffer);
                memcpy(DataAddress, KotSpecificDataClient, sizeof(kot_SpecificData_t));
            }
        }else{
            kot_Printlog("WARNING : This ELF seems not to be compatible with KOT");
        }
        for(uint64_t i = 0; i < self->Header->e_phnum; i++){
            Elf64_Phdr* phdr = (Elf64_Phdr*)((uint64_t)self->phdrs + (i * self->Header->e_phentsize));
            if(phdr->p_type == PT_LOAD){
                void* TmpAddress = (void*)malloc(phdr->p_memsz);
                memset((void*)TmpAddress, 0x0, phdr->p_memsz);  
                memcpy((void*)TmpAddress, (void*)((uint64_t)buffer + phdr->p_offset), phdr->p_filesz);  

                kot_key_mem_t SharedKey = NULL;
                uint64_t flags = 0;

                kot_Sys_CreateMemoryField(parentProcess, phdr->p_memsz, &TmpAddress, &SharedKey, MemoryFieldTypeSendSpaceRO);
                void* clientAddress = (void*)phdr->p_vaddr;

                kot_Sys_AcceptMemoryField(proc, SharedKey, &clientAddress);
                kot_Sys_CloseMemoryField(parentProcess, SharedKey, TmpAddress);
                free((void*)TmpAddress);
            }
        }

        free(KotSpecificDataClient);
        free(self);
        return KSUCCESS;

    }

    bool Check(elf_t* self) {
        return (self->Header->e_ident[EI_MAG0] != ELFMAG0 || self->Header->e_ident[EI_MAG1] != ELFMAG1 || self->Header->e_ident[EI_MAG2] != ELFMAG2 || self->Header->e_ident[EI_MAG3] != ELFMAG3);
    }
}

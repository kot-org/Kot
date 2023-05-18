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
            if(strcmp(GetSectionName(self, shdr), name)){
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

    KResult loadElf(uintptr_t buffer, enum Priviledge privilege, uint64_t identifier, thread_t* mainthread, char* rootpath, bool isVFS) {
        elf_t* self = (elf_t*)calloc(sizeof(elf_t));
        self->Buffer = buffer;
        self->Header = (Elf64_Ehdr*)buffer;
        
        /* Check elf */
        if(Check(self)){
            free(self);
            return KFAIL;
        }
        
        process_t proc = NULL;
        
        if(Sys_CreateProc(&proc, privilege, identifier) != KSUCCESS) return KFAIL;
        /* TODO : create thread identifier */
        if(Sys_CreateThread(proc, (uintptr_t)self->Header->e_entry, privilege, NULL, mainthread) != KSUCCESS) return KFAIL;

        process_t parentProcess = Sys_GetProcess();

        /* Load the elf */
        self->phdrs = (uintptr_t)((uint64_t)buffer + self->Header->e_phoff);
        self->shdrs = (uintptr_t)((uint64_t)buffer + self->Header->e_shoff);
        
        self->shstr = GetSectionHeaderIndex(self, self->Header->e_shstrndx);
        self->KotSpecific = GetSectionHeaderName(self, ".KotSpecificData");

        thread_t Runningthread = Sys_GetThread();

        uint64_t HeapLocation = 0x0;
        for(uint64_t i = 0; i < self->Header->e_phnum; i++){
            Elf64_Phdr* phdr = (Elf64_Phdr*)((uint64_t)self->phdrs + (i * self->Header->e_phentsize));
            if((phdr->p_vaddr + phdr->p_memsz) > HeapLocation){
                HeapLocation = phdr->p_vaddr + phdr->p_memsz;
            }
        }
        KotSpecificData_t* KotSpecificDataClient = NULL;
        if(self->KotSpecific != NULL){
            /* Check symbol size */
            if(self->KotSpecific->sh_size >= sizeof(KotSpecificData_t)){
                if(self->KotSpecific->sh_size != sizeof(KotSpecificData_t)){
                    Printlog("WARNING : This ELF might be corrupter OR doesn't respect KOT specifications");
                }
                if(HeapLocation % KotSpecificData.MMapPageSize){
                    HeapLocation -= HeapLocation % KotSpecificData.MMapPageSize;
                    HeapLocation += KotSpecificData.MMapPageSize;
                }
                KotSpecificDataClient = (KotSpecificData_t*)calloc(sizeof(KotSpecificData_t));
                KotSpecificDataClient->MMapPageSize = KotSpecificData.MMapPageSize;
                KotSpecificDataClient->HeapLocation = HeapLocation;
                KotSpecificDataClient->FreeMemorySpace = KotSpecificData.FreeMemorySpace;
                KotSpecificDataClient->UISDHandler = KotSpecificData.UISDHandler;

                if(isVFS){
                    srv_storage_callback_t* Callback = Srv_Storage_VFSLoginApp(ShareProcessKey(proc), FS_AUTHORIZATION_HIGH, Storage_Permissions_Admin | Storage_Permissions_Read | Storage_Permissions_Write | Storage_Permissions_Create, rootpath, true);
                    KotSpecificDataClient->VFSHandler = Callback->Data;
                    free(Callback);
                }
                uintptr_t DataAddress = (uintptr_t)(self->KotSpecific->sh_offset + (uint64_t)self->Buffer);
                memcpy(DataAddress, KotSpecificDataClient, sizeof(KotSpecificData_t));
            }
        }else{
            Printlog("WARNING : This ELF seems not to be compatible with KOT");
        }

        for(uint64_t i = 0; i < self->Header->e_phnum; i++){
            Elf64_Phdr* phdr = (Elf64_Phdr*)((uint64_t)self->phdrs + (i * self->Header->e_phentsize));
            if(phdr->p_type == PT_LOAD){
                uintptr_t TmpAddress = (uintptr_t)malloc(phdr->p_memsz);
                memset((uintptr_t)TmpAddress, 0x0, phdr->p_memsz);  
                memcpy((uintptr_t)TmpAddress, (uintptr_t)((uint64_t)buffer + phdr->p_offset), phdr->p_filesz);  

                ksmem_t SharedKey = NULL;
                uint64_t flags = 0;

                Sys_CreateMemoryField(parentProcess, phdr->p_memsz, &TmpAddress, &SharedKey, MemoryFieldTypeSendSpaceRO);
                uintptr_t clientAddress = (uintptr_t)phdr->p_vaddr;

                Sys_AcceptMemoryField(proc, SharedKey, &clientAddress);
                Sys_CloseMemoryField(parentProcess, SharedKey, TmpAddress);
                free((uintptr_t)TmpAddress);
            }
        }

        free(KotSpecificDataClient);
        free(self);

        return KSUCCESS;

    }

    bool Check(elf_t* self) {
        return (self->Header->e_ident[0] != EI_MAG0 || self->Header->e_ident[1] != EI_MAG1 || self->Header->e_ident[2] != EI_MAG2 || self->Header->e_ident[3] != EI_MAG3);
    }
}

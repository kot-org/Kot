#include <elf/elf.h>

namespace ELF{
    KResult loadElf(void* buffer, uint8_t ring, parameters_t* functionParameters, uint64_t identifier){
        elf_t* self = (elf_t*)calloc(sizeof(elf_t));
        self->Buffer = buffer;
        self->Header = (Elf64_Ehdr*)buffer;
        
        /* Check elf */
        if(Check(self)){
            free(self);
            return KFAIL;
        }
        
        kprocess_t proc = NULL;
        kthread_t mainThread = NULL;
        if(Sys_CreatProc(&proc, ring, identifier) != KSUCCESS) return KFAIL;
        /* TODO : creat thread identifier */
        if(Sys_CreatThread(proc, (void*)self->Header->e_entry, 0x0, ring, &mainThread) != KSUCCESS) return KFAIL;
        
        /* Load the elf */
        void* phdrs = (void*)(uint64_t)buffer + self->Header->e_phoff;

        kthread_t RunningThread = NULL;
        SYS_GetThreadKey(&RunningThread);

        for(int i = 0; i < self->Header->e_phnum; i++){
            Elf64_Phdr* phdr = (Elf64_Phdr*)((uint64_t)phdrs + (i * self->Header->e_phentsize));
            if(phdr->p_type == PT_LOAD){
                uintptr_t TmpAddress = (uintptr_t)malloc(phdr->p_memsz);
                memset((void*)TmpAddress, 0x0, phdr->p_memsz);  
                memcpy((void*)TmpAddress, (void*)((uint64_t)buffer + phdr->p_offset), phdr->p_filesz);  

                ksmem_t SharedKey = NULL;
                uint64_t flags = 0;
                memory_share_flag_SetFlag(&flags, memory_share_flag_NLA, true);
                SYS_CreatShareSpace(RunningThread, phdr->p_memsz, &TmpAddress, &SharedKey, flags);
                uintptr_t clientAddress = (uintptr_t)phdr->p_vaddr;
                SYS_GetShareSpace(mainThread, SharedKey, &clientAddress);
                free(TmpAddress);
                SYS_FreeShareSpace(RunningThread, SharedKey, TmpAddress);
            }
        }

        Sys_ExecThread(mainThread, functionParameters);
        free(self);
        return KSUCCESS;
    }

    bool Check(elf_t* self){
        return (self->Header->e_ident[0] != EI_MAG0 || self->Header->e_ident[1] != EI_MAG1 || self->Header->e_ident[2] != EI_MAG2 || self->Header->e_ident[3] != EI_MAG3);
    }
}

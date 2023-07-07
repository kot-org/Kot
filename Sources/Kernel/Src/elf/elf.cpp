#include <elf/elf.h>
#include <abi-bits/vm-flags.h>

namespace ELF{
    static inline char* GetSectionName(elf_t* self, Elf64_Shdr* shdr){
        return (char*)((uint64_t)self->Buffer + self->shstr->sh_offset + shdr->sh_name);
    }
 
    static inline Elf64_Shdr* GetSectionHeaderIndex(elf_t* self, Elf64_Half index){
        return (Elf64_Shdr*)((uint64_t)self->shdrs + (index * self->Header->e_shentsize));
    }
    static inline Elf64_Shdr* GetSectionHeaderName(elf_t* self, char* name){
        for(Elf64_Half i = 0; i <  self->Header->e_shnum; i++){
            Elf64_Shdr* shdr = GetSectionHeaderIndex(self, i);
            if(strcmp(GetSectionName(self, shdr), name)){
                return shdr;
            }
        }
        return NULL;
    }
    static inline Elf64_Shdr* GetSectionHeaderType(elf_t* self, uint32_t type){
        for(Elf64_Half i = 0; i < self->Header->e_shnum; i++){
            Elf64_Shdr* shdr = GetSectionHeaderIndex(self, i);
            if(shdr->sh_type == type){
                return shdr;
            }
        }
        return NULL;
    }

    static inline Elf64_Half GetSectionIndex(elf_t* self, Elf64_Shdr* shdr){
        return (Elf64_Half)(((uint64_t)shdr - (uint64_t)self->shdrs) / self->Header->e_shentsize);
    }

    static inline Elf64_Sym* GetSymbol(elf_t* self, Elf64_Half sectionIndex, char* name){
        uint64_t size = self->symtab->sh_size / sizeof(Elf64_Sym);
        for(uint64_t i = 0; i < size; i++){
            Elf64_Sym* sym = (Elf64_Sym*)((uint64_t)self->Buffer + self->symtab->sh_offset + i * sizeof(Elf64_Sym));
            if(sym->st_shndx == sectionIndex){
                if(strcmp((char*)((uint64_t)self->Buffer + self->str->sh_offset + sym->st_name), name)){
                    return sym;
                }
            }
        }
    }


    KResult loadElf(void* buffer, enum Priviledge ring, kthread_t** selfthread){
        elf_t* self = (elf_t*)kcalloc(sizeof(elf_t));
        self->Buffer = buffer;
        self->Header = (Elf64_Ehdr*)buffer;
        
        /* Check elf */
        if(Check(self)){
            kfree(self);
            return KFAIL;
        }
        
        kprocess_t* proc = NULL;
        globalTaskManager->CreateProcess(&proc, ring, 0);
        kthread_t* mainthread = proc->Createthread((void*)self->Header->e_entry, NULL);
        
        /* Load the elf */
        self->phdrs = (void*)((uint64_t)buffer + self->Header->e_phoff);
        self->shdrs = (void*)((uint64_t)buffer + self->Header->e_shoff);
        
        self->shstr = GetSectionHeaderIndex(self, self->Header->e_shstrndx);
        self->str = GetSectionHeaderName(self, ".strtab");
        self->symtab = GetSectionHeaderType(self, SHT_SYMTAB);
        self->KotSpecific = GetSectionHeaderName(self, ".KotSpecificData");

        uint64_t HeapLocation = 0x0;
        for(Elf64_Half i = 0; i < self->Header->e_phnum; i++){
            Elf64_Phdr* phdr = (Elf64_Phdr*)((uint64_t)self->phdrs + (i * self->Header->e_phentsize));
            if((phdr->p_vaddr + phdr->p_memsz) > HeapLocation){
                HeapLocation = phdr->p_vaddr + phdr->p_memsz;
            }
            if(phdr->p_type == PT_LOAD){
                size64_t align = phdr->p_vaddr & 0xFFF; // Get last 9 bits
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

                    void* physicalPage = NULL;
                    if(!vmm_GetFlags(mainthread->Paging, (void*)virtualAddressIterator, vmm_Present)){
                        physicalPage = Pmm_RequestPage();
                        vmm_Map(mainthread->Paging, (void*)((uint64_t)virtualAddressIterator), physicalPage, true, true, true);
                        physicalPage = (void*)((uint64_t)physicalPage + alignement);
                    }else{
                        physicalPage = vmm_GetPhysical(mainthread->Paging, (void*)virtualAddressIterator);
                    }
                    if(totalSizeCopy < phdr->p_filesz){
                        memcpy((void*)(vmm_GetVirtualAddress(physicalPage)), (void*)virtualAddressParentIterator, sizeToCopy);
                    }else{
                        memset((void*)(vmm_GetVirtualAddress(physicalPage)), NULL, sizeToCopy);
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
                
                    void* physicalPage = NULL;
                    if(!vmm_GetFlags(mainthread->Paging, (void*)virtualAddressIterator, vmm_Present)){
                        physicalPage = Pmm_RequestPage();
                        vmm_Map(mainthread->Paging, (void*)((uint64_t)virtualAddressIterator), physicalPage, true, true, true);
                    }else{
                        physicalPage = vmm_GetPhysical(mainthread->Paging, (void*)virtualAddressIterator);
                    }
                    if(totalSizeCopy < phdr->p_filesz){
                        memcpy((void*)(vmm_GetVirtualAddress(physicalPage)), (void*)virtualAddressParentIterator, sizeToCopy);
                    }else{
                        memset((void*)(vmm_GetVirtualAddress(physicalPage)), NULL, sizeToCopy);
                    }

                    virtualAddressIterator += sizeToCopy;
                    virtualAddressParentIterator += sizeToCopy;
                    totalSizeCopy += sizeToCopy;
                    size -= sizeToCopy;
                }
            }
        }

        if(self->KotSpecific != NULL){
            self->KotSpecificIndex = GetSectionIndex(self, self->KotSpecific);

            self->KotSpecificSymbol = GetSymbol(self, self->KotSpecificIndex, "KotSpecificData");

            /* Fill KotSpecific */
            if(self->KotSpecificSymbol != NULL){
                /* Check symbol size */
                if(self->KotSpecificSymbol->st_size >= sizeof(kot_SpecificData_t)){
                    if(HeapLocation % PAGE_SIZE){
                        HeapLocation -= HeapLocation % PAGE_SIZE;
                        HeapLocation += PAGE_SIZE;
                    }
                    kot_SpecificData_t* KotSpecificData = (kot_SpecificData_t*)kcalloc(sizeof(kot_SpecificData_t));
                    KotSpecificData->MMapPageSize = PAGE_SIZE;
                    KotSpecificData->HeapLocation = HeapLocation;
                    KotSpecificData->FreeMemorySpace = (void*)FREE_MEMORY_SPACE_ADDRESS;
                    void* DataAddress = (void*)vmm_GetVirtualAddress(vmm_GetPhysical(mainthread->Paging, (void*)self->KotSpecificSymbol->st_value));
                    memcpy(DataAddress, KotSpecificData, sizeof(kot_SpecificData_t));
                    kfree(KotSpecificData);
                }
            }
        }

        size_t ProtectedSize =  HeapLocation - (uintptr_t)proc->MemoryManager->Base;
        int Errno = 0;
        MemoryRegion_t* Region = MMAllocateRegionVM(proc->MemoryManager, proc->MemoryManager->Base, ProtectedSize, MAP_FIXED, PROT_READ | PROT_WRITE | PROT_EXEC, &Errno);

        if(!Region){
            return KFAIL;
        }

        *selfthread = mainthread;
        kfree(self);
        return KSUCCESS;
    }

    bool Check(elf_t* self){
        return (self->Header->e_ident[0] != ELFMAG0 || self->Header->e_ident[1] != ELFMAG1 || self->Header->e_ident[2] != ELFMAG2 || self->Header->e_ident[3] != ELFMAG3);
    }
}

// #include <elf/elf.h>

// namespace ELF{
//     KResult loadElf(void* buffer, uint8_t ring, Parameters* FunctionParameters){
//         elf_t* self = (elf_t*)calloc(sizeof(elf_t));
//         self->Buffer = buffer;
//         self->Header = (Elf64_Ehdr*)buffer;
        
//         /* Check elf */
//         if(Check(self)){
//             free(self);
//             return KFAIL;
//         }
        
//         process_t* proc = NULL;
//         globalTaskManager->CreatProcess(&proc, ring, 0);
//         thread_t* mainThread = proc->CreatThread((uint64_t)self->Header->e_entry, NULL);

//         LoadSections(self);

//         uint64_t Address = GetLastAddressUsed(self);

//         if(self->DynSH != NULL){
//             Elf64_Dyn* dyn = (Elf64_Dyn*)((uint64_t)buffer + self->DynSH->sh_offset);
//             uint64_t num = self->DynSH->sh_size / self->DynSH->sh_entsize;
            
//             for(uint64_t i = 0; i < num; i++){
//                 if(dyn->d_tag == DT_NEEDED){
//                     char* name = (char*)((uint64_t)buffer + (uint64_t)self->DynstrSH->sh_offset + (uint64_t)dyn->d_un.d_ptr);

//                     /* Load library file */
//                     RamFS::File* LibFile = RamFS::Find(name);
//                     void* BufferLib = malloc(LibFile->size);
//                     Read(LibFile, BufferLib);

//                     /* Load library */
//                     LoadLibrary(mainThread->Paging, self, BufferLib, &Address);
//                 }

//                 dyn = (Elf64_Dyn*)((uint8_t*)dyn + self->DynSH->sh_entsize);
//             }
//         }
        
//         /* Load the elf */
//         LoadBinary(mainThread->Paging, self, 0);
        
//         mainThread->Launch(FunctionParameters);
//         return KSUCCESS;
//     }

//     KResult LoadLibrary(pagetable_t paging, elf_t* client, void* buffer, uint64_t* address){
//         elf_t* self = (elf_t*)calloc(sizeof(elf_t));
//         self->Buffer = buffer;
//         self->Header = (Elf64_Ehdr*)buffer;
        
//         /* Check elf */
//         if(Check(self)){
//             free(self);
//             return KFAIL;
//         }
        
//         LoadSections(self);

//         /* Map local symbols */

//         Elf64_Shdr* shdrSelf = (Elf64_Shdr*)((uint64_t)self->Buffer + (uint64_t)self->Header->e_shoff);
//         Elf64_Shdr* shdrClient = (Elf64_Shdr*)((uint64_t)client->Buffer + (uint64_t)client->Header->e_shoff);

//         if(self->GotSH != NULL){
//             for(uint64_t i = 0; i < self->Header->e_shnum; i++){
//                 if(shdrSelf->sh_type == SHT_RELA){
//                     Elf64_Rela* rela = (Elf64_Rela*)((uint64_t)self->Buffer + shdrSelf->sh_offset);
//                     uint64_t NumLib = shdrSelf->sh_size / shdrSelf->sh_entsize;
                    
//                     for(uint64_t i = 0; i < NumLib; i++){
//                         Elf64_Sym* SymLib = (Elf64_Sym*)((uint64_t)self->Buffer + self->DynsymSH->sh_offset + (ELF64_R_SYM(rela->r_info) * self->DynsymSH->sh_entsize));
//                         char* symbolLib = (char*)((uint64_t)self->Buffer + self->DynstrSH->sh_offset + SymLib->st_name);

//                         /* Set symbol entry in GOT */
//                         uint64_t* GOTPointer = (uint64_t*)((uint64_t)self->Buffer + (uint64_t)self->GotSH->sh_offset + (2 + ELF64_R_SYM(rela->r_info)) * self->GotSH->sh_entsize);
//                         *GOTPointer = SymLib->st_value + *address;

//                         rela = (Elf64_Rela*)((uint8_t*)rela + shdrSelf->sh_entsize);
//                     }     
//                 }
//                 shdrSelf = (Elf64_Shdr*)((uint8_t*)shdrSelf + self->Header->e_shentsize);         
//             }
//         }


//         /* Get app symbol need */
//         if(self->DynsymSH != NULL && client->GotSH != NULL){
//             for(uint64_t i = 0; i < client->Header->e_shnum; i++){
//                 if(shdrClient->sh_type == SHT_RELA){
//                     Elf64_Rela* rela = (Elf64_Rela*)((uint64_t)client->Buffer + shdrClient->sh_offset);
//                     uint64_t num = shdrClient->sh_size / shdrClient->sh_entsize;
                    
//                     for(uint64_t i = 0; i < num; i++){
//                         Elf64_Sym* Symbol = (Elf64_Sym*)((uint64_t)client->Buffer + client->DynsymSH->sh_offset + (ELF64_R_SYM(rela->r_info) * client->DynsymSH->sh_entsize));
//                         char* SymbolChar = (char*)((uint64_t)client->Buffer + client->DynstrSH->sh_offset + Symbol->st_name);
                        
//                         /* Now find it in the library */
//                         Elf64_Sym* LibSymbol = (Elf64_Sym*)((uint64_t)self->Buffer + self->DynsymSH->sh_offset);
//                         uint64_t NumLib = self->DynsymSH->sh_size / self->DynsymSH->sh_entsize;
                        
//                         for(uint64_t i = 0; i < NumLib; i++){
//                             char* SymbolLibChar = (char*)((uint64_t)self->Buffer + self->DynstrSH->sh_offset + LibSymbol->st_name);

//                             if(strcmp(SymbolChar, SymbolLibChar)){
//                                 /* Let's link it */
//                                 uint64_t* GOTPointer = (uint64_t*)((uint64_t)client->Buffer + (uint64_t)client->GotSH->sh_offset + (2 + ELF64_R_SYM(rela->r_info)) * client->GotSH->sh_entsize);
//                                 *GOTPointer = LibSymbol->st_value + *address;
//                                 break;
//                             }

//                             LibSymbol = (Elf64_Sym*)((uint8_t*)LibSymbol + self->DynsymSH->sh_entsize);
//                         }

//                         rela = (Elf64_Rela*)((uint8_t*)rela + shdrClient->sh_entsize);
//                     }  
//                 }
//                 shdrClient = (Elf64_Shdr*)((uint8_t*)shdrClient + client->Header->e_shentsize);
//             }
//         }

//         LoadBinary(paging, self, *address);

//         *address = GetLastAddressUsed(self);

//         return KSUCCESS;
//     }

//     bool Check(elf_t* self){
//         return (self->Header->e_ident[0] != EI_MAG0 || self->Header->e_ident[1] != EI_MAG1 || self->Header->e_ident[2] != EI_MAG2 || self->Header->e_ident[3] != EI_MAG3);
//     }

//     void LoadSections(elf_t* self){
//         /* Get location data */
//         Elf64_Shdr* shdr = (Elf64_Shdr*)((uint64_t)self->Buffer + (uint64_t)self->Header->e_shoff);
//         Elf64_Shdr* strt = (Elf64_Shdr*)((uint64_t)shdr + ((uint64_t)self->Header->e_shstrndx * (uint64_t)self->Header->e_shentsize));

//         char* strtable = (char*)((uint64_t)self->Buffer + strt->sh_offset);

//         /* Read sections */
//         for(uint64_t i = 0; i < self->Header->e_shnum; i++){
//             char* name = (char*)((uint64_t)strtable + (uint64_t)shdr->sh_name);
//             if(shdr->sh_type == SHT_DYNAMIC){
//                 self->DynSH = shdr;
//             }
//             if(strcmp(name, ".got.plt")){
//                 self->GotSH = shdr;
//             }
//             if(shdr->sh_type == SHT_DYNSYM){
//                 self->DynsymSH = shdr;
//             }
//             if(shdr->sh_type == SHT_SYMTAB){
//                 self->SymSH = shdr;
//             }
//             if(shdr->sh_type == SHT_STRTAB && strcmp(name, ".dynstr")){
//                 self->DynstrSH = shdr;
//             }

//             shdr = (Elf64_Shdr*)((uint8_t*)shdr + self->Header->e_shentsize);
//         }
//     }

//     uint64_t GetLastAddressUsed(struct elf_t* self){
//         uint64_t ReturnValue = 0;
//         void* phdrs = (void*)((uint64_t)self->Buffer + (uint64_t)self->Header->e_phoff);
//         for(int i = 0; i < self->Header->e_phnum; i++){
//             Elf64_Phdr* phdr = (Elf64_Phdr*)((uint64_t)phdrs + (i * self->Header->e_phentsize));
//             if(phdr->p_type == PT_LOAD){
//                 if(phdr->p_vaddr + phdr->p_memsz){
//                     ReturnValue = phdr->p_vaddr + phdr->p_memsz;
//                 }
//             }
//         }

//         if(ReturnValue % PAGE_SIZE != 0){
//             ReturnValue -= ReturnValue % PAGE_SIZE;
//             ReturnValue += PAGE_SIZE;
//         }
//         return ReturnValue;
//     }

//     void LoadBinary(pagetable_t table, struct elf_t* self, uint64_t address){
//         void* phdrs = (void*)((uint64_t)self->Buffer + (uint64_t)self->Header->e_phoff);
//         for(int i = 0; i < self->Header->e_phnum; i++){
//             Elf64_Phdr* phdr = (Elf64_Phdr*)((uint64_t)phdrs + (i * self->Header->e_phentsize));
//             switch (phdr->p_type){
//                 case PT_LOAD:
//                 {	
//                     Elf64_Addr segment = phdr->p_vaddr + address;

//                     uint64_t pages = Divide(phdr->p_memsz, PAGE_SIZE);
//                     uint64_t size = phdr->p_filesz;
//                     for(uint64_t y = 0; y < pages; y++){
//                         uint64_t SizeToCopy = 0;
//                         if(size > PAGE_SIZE){
//                             SizeToCopy = PAGE_SIZE; 
//                         }else{
//                             SizeToCopy = size;
//                         }
//                         size -= SizeToCopy; 
//                         void* virtualAddress = (void*)(segment + y * PAGE_SIZE);
//                         uint64_t offset = (uint64_t)virtualAddress % PAGE_SIZE;
//                         virtualAddress -= offset;
//                         //Custom 0 flags : is user executable
//                         if(!vmm_GetFlags(table, virtualAddress, vmm_flag::vmm_Custom0)){
//                             void* PhysicalBuffer = Pmm_RequestPage();
//                             vmm_Map(table, (void*)virtualAddress, (void*)PhysicalBuffer, true);
//                             vmm_SetFlags(table, virtualAddress, vmm_flag::vmm_Custom0, true);
//                             memcpy((void*)vmm_GetVirtualAddress(PhysicalBuffer) + offset, (void*)((uint64_t)self->Buffer + phdr->p_offset), SizeToCopy);
//                         }
//                     }
//                     break;   
//                 }
//             }
//         }
//     }
// }

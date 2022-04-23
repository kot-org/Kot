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
        
//         kprocess_t proc = NULL;
//         globalTaskManager->CreatProcess(&proc, ring, 0);
//         kthread_t mainThread = proc->CreatThread((uint64_t)self->Header->e_entry, NULL);

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

//     KResult LoadLibrary(kthread_t thread, elf_t* client, void* buffer, uint64_t* address){
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

//         if(self->GotSH != NULL){
//             Elf64_Rela* rela = (Elf64_Rela*)((uint64_t)self->Buffer + self->RelaSH->sh_offset);
//             uint64_t NumLib = self->RelaSH->sh_size / self->RelaSH->sh_entsize;
            
//             for(uint64_t i = 0; i < NumLib; i++){
//                 Elf64_Sym* SymLib = (Elf64_Sym*)((uint64_t)self->Buffer + self->DynsymSH->sh_offset + (ELF64_R_SYM(rela->r_info) * self->DynsymSH->sh_entsize));
//                 char* symbolLib = (char*)((uint64_t)self->Buffer + self->DynstrSH->sh_offset + SymLib->st_name);

//                 /* Update symbol entry */
//                 SymLib->st_value += *address;

//                 /* Set symbol entry in GOT */
//                 uint64_t* GOTPointer = (uint64_t*)((uint64_t)self->Buffer + (uint64_t)self->GotSH->sh_offset + (2 + ELF64_R_SYM(rela->r_info)) * self->GotSH->sh_entsize);
//                 *GOTPointer = SymLib->st_value;

//                 rela = (Elf64_Rela*)((uint8_t*)rela + self->RelaSH->sh_entsize);
//             }
//         }
// 
// /* Rela */
// if(client->RelaSH != NULL && self->RelaSH != NULL){
//     Elf64_Rela* rela = (Elf64_Rela*)((uint64_t)client->Buffer + client->RelaSH->sh_offset);
//     uint64_t num = client->RelaSH->sh_size / client->RelaSH->sh_entsize;
    
//     for(uint64_t i = 0; i < num; i++){
//         Elf64_Sym* sym = (Elf64_Sym*)((uint64_t)client->Buffer + client->DynsymSH->sh_offset + (ELF64_R_SYM(rela->r_info) * client->DynsymSH->sh_entsize));
//         char* symbol = (char*)((uint64_t)client->Buffer + client->DynstrSH->sh_offset + sym->st_name);
        
//         /* Now find it in the library */
//         Elf64_Rela* relaLib = (Elf64_Rela*)((uint64_t)self->Buffer + self->RelaSH->sh_offset);
//         uint64_t NumLib = self->RelaSH->sh_size / self->RelaSH->sh_entsize;
        
//         for(uint64_t i = 0; i < NumLib; i++){
//             Elf64_Sym* SymLib = (Elf64_Sym*)((uint64_t)self->Buffer + self->DynsymSH->sh_offset + (ELF64_R_SYM(relaLib->r_info) * self->DynsymSH->sh_entsize));
//             char* symbolLib = (char*)((uint64_t)self->Buffer + self->DynstrSH->sh_offset + SymLib->st_name);
//             if(strcmp(symbol, symbolLib)){
//                 /* Let's link it */
//                 uint64_t* GOTPointer = (uint64_t*)((uint64_t)client->Buffer + (uint64_t)client->GotSH->sh_offset + (2 + ELF64_R_SYM(rela->r_info)) * client->GotSH->sh_entsize);
//                 *GOTPointer = SymLib->st_value;
//             }
//             relaLib = (Elf64_Rela*)((uint8_t*)relaLib + self->RelaSH->sh_entsize);
//         }
//         rela = (Elf64_Rela*)((uint8_t*)rela + client->RelaSH->sh_entsize);
//     }            
// } 
// /* Relat */
// if(client->RelatSH != NULL && self->RelatSH != NULL){
//     Elf64_Rela* rela = (Elf64_Rela*)((uint64_t)client->Buffer + client->RelatSH->sh_offset);
//     uint64_t num = client->RelatSH->sh_size / client->RelatSH->sh_entsize;
    
//     for(uint64_t i = 0; i < num; i++){
//         Elf64_Sym* sym = (Elf64_Sym*)((uint64_t)client->Buffer + client->DynsymSH->sh_offset + (ELF64_R_SYM(rela->r_info) * client->DynsymSH->sh_entsize));
//         char* symbol = (char*)((uint64_t)client->Buffer + client->DynstrSH->sh_offset + sym->st_name);
        
//         /* Now find it in the library */
//         Elf64_Rela* relaLib = (Elf64_Rela*)((uint64_t)self->Buffer + self->RelatSH->sh_offset);
//         uint64_t NumLib = self->RelatSH->sh_size / self->RelatSH->sh_entsize;
        
//         for(uint64_t i = 0; i < NumLib; i++){
//             Elf64_Sym* SymLib = (Elf64_Sym*)((uint64_t)self->Buffer + self->DynsymSH->sh_offset + (ELF64_R_SYM(relaLib->r_info) * self->DynsymSH->sh_entsize));
//             char* symbolLib = (char*)((uint64_t)self->Buffer + self->DynstrSH->sh_offset + SymLib->st_name);
//             if(strcmp(symbol, symbolLib)){
//                 /* Let's link it */
//                 uint64_t* GOTPointer = (uint64_t*)((uint64_t)client->Buffer + (uint64_t)client->GotSH->sh_offset + (2 + ELF64_R_SYM(rela->r_info)) * client->GotSH->sh_entsize);
//                 *GOTPointer = SymLib->st_value;
//             }
//             relaLib = (Elf64_Rela*)((uint8_t*)relaLib + self->RelatSH->sh_entsize);
//         }
//         rela = (Elf64_Rela*)((uint8_t*)rela + client->RelatSH->sh_entsize);
//     }            
// } 

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
//             if(strcmp(name, ".rela.plt")){
//                 self->RelaSH = shdr;
//             }
//             if(strcmp(name, ".rela.text") || strcmp(name, ".rela.dyn")){
//                 self->RelatSH = shdr;
//             }
//             if(strcmp(name, ".dynamic")){
//                 self->DynSH = shdr;
//             }
//             if(strcmp(name, ".got.plt")){
//                 self->GotSH = shdr;
//             }
//             if(strcmp(name, ".dynsym")){
//                 self->DynsymSH = shdr;
//             }
//             if(strcmp(name, ".symtab")){
//                 self->SymSH = shdr;
//             }
//             if(strcmp(name, ".dynstr")){
//                 self->DynstrSH = shdr;
//             }
//             if(strcmp(name, ".strtab")){
//                 self->StrSH = shdr;
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

//     void LoadBinary(kthread_t thread, struct elf_t* self, uint64_t address){
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

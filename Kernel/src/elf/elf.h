#pragma once
#include <elf/data.h>
#include <main/kernelInit.h>

namespace ELF{
    /* Function */
    KResult loadElf(void* buffer, uint8_t ring, struct Parameters* FunctionParameters);
    KResult LoadLibrary(pagetable_t paging, struct elf_t* app, void* buffer);

    bool Check(struct elf_t* self);
    void LoadSections(struct elf_t* self);
    uint64_t GetLastAddressUsed(struct elf_t* self);
    void LoadElf(pagetable_t table, struct elf_t* self);

    struct elf_t{
        void* Buffer;
        Elf64_Ehdr* Header;
        Elf64_Shdr* RelaSH;
        Elf64_Shdr* RelatSH;
        Elf64_Shdr* DynSH;
        Elf64_Shdr* GotSH;
        Elf64_Shdr* DynsymSH;
        Elf64_Shdr* DynstrSH;
        Elf64_Shdr* SymSH;
        Elf64_Shdr* StrSH;
    };
}
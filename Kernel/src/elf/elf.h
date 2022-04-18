#pragma once
#include <kot/elf.h>
#include <main/kernelInit.h>

namespace ELF{
    /* Function */
    KResult loadElf(void* buffer, uint8_t ring, struct Parameters* FunctionParameters);
    KResult LoadLibrary(pagetable_t paging, struct elf_t* app, void* buffer, uint64_t* address);

    bool Check(struct elf_t* self);
    void LoadSections(struct elf_t* self);
    uint64_t GetLastAddressUsed(struct elf_t* self);
    void LoadBinary(pagetable_t table, struct elf_t* self, uint64_t address);

    struct elf_t{
        void* Buffer;
        Elf64_Ehdr* Header;
    };
}
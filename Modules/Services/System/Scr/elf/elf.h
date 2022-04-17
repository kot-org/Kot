#pragma once
#include <kot/elf.h>
#include <kot/cstring.h>
#include <kot/sys/sys.h>

namespace ELF{
    /* Function */
    KResult loadElf(void* buffer, uint8_t ring, struct Parameters* FunctionParameters);
    KResult LoadLibrary(kthread_t thread, struct elf_t* app, void* buffer, uint64_t* address);

    bool Check(struct elf_t* self);
    void LoadSections(struct elf_t* self);
    uint64_t GetLastAddressUsed(struct elf_t* self);
    void LoadBinary(kthread_t thread, struct elf_t* self, uint64_t address);

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
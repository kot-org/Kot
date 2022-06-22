#pragma once
#include <kot/elf.h>
#include <kot/sys.h>
#include <kot/heap.h>
#include <kot/math.h>
#include <kot/cstring.h>
#include <kot/utils.h>

#include <main/main.h>

namespace ELF{
    /* Function */
    KResult loadElf(uintptr_t buffer, uint8_t ring, uint64_t identifier, kthread_t* mainThread);

    bool Check(struct elf_t* self);

    struct elf_t{
        uintptr_t Buffer;
        struct Elf64_Ehdr* Header;
        uintptr_t phdrs;
        uintptr_t shdrs;
        struct Elf64_Shdr* str;
        struct Elf64_Shdr* shstr;
        struct Elf64_Shdr* symtab;
        struct Elf64_Shdr* KotSpecific;
        Elf64_Half KotSpecificIndex;
        struct Elf64_Sym* KotSpecificSymbol;
    };
}
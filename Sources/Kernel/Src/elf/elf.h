#pragma once
#include <lib/elf.h>
#include <core/core.h>
#include <scheduler/scheduler.h>

namespace ELF{
    /* Function */
    KResult loadElf(uintptr_t buffer, enum Priviledge ring, struct kthread_t** selfthread);

    bool Check(struct elf_t* self);

    struct elf_t{
        uintptr_t Buffer;
        Elf64_Ehdr* Header;
        uintptr_t phdrs;
        uintptr_t shdrs;
        Elf64_Shdr* str;
        Elf64_Shdr* shstr;
        Elf64_Shdr* symtab;
        Elf64_Shdr* KotSpecific;
        Elf64_Half KotSpecificIndex;
        Elf64_Sym* KotSpecificSymbol;
    };
}
#pragma once
#include <kot/elf.h>
#include <core/core.h>

namespace ELF{
    /* Function */
    KResult loadElf(void* buffer, uint8_t ring, struct Parameters* FunctionParameters);

    bool Check(struct elf_t* self);

    struct elf_t{
        void* Buffer;
        Elf64_Ehdr* Header;
    };
}
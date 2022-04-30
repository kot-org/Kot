#pragma once
#include <kot/elf.h>
#include <kot/sys.h>
#include <kot/heap.h>
#include <kot/math.h>
#include <kot/cstring.h>

#include <main/main.h>

namespace ELF{
    /* Function */
    KResult loadElf(void* buffer, uint8_t ring, parameters_t* functionParameters, uint64_t identifier);

    bool Check(struct elf_t* self);

    struct elf_t{
        void* Buffer;
        Elf64_Ehdr* Header;
    };
}
#pragma once
#include <kot/elf.h>
#include <kot/sys.h>
#include <kot/heap.h>
#include <kot/cstring.h>

namespace ELF{
    /* Function */
    KResult loadElf(void* buffer, uint8_t ring, struct Parameters* FunctionParameters);
    KResult LoadLibrary(kthread_t thread, struct elf_t* app, void* buffer, uint64_t* address);

    bool Check(struct elf_t* self);

    struct elf_t{
        void* Buffer;
        Elf64_Ehdr* Header;
    };
}
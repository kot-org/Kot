#pragma once
#include <kot/elf.h>
#include <core/core.h>
#include <scheduler/scheduler.h>

namespace ELF{
    /* Function */
    KResult loadElf(void* buffer, uint8_t ring, struct thread_t** selfThread);

    bool Check(struct elf_t* self);

    struct elf_t{
        void* Buffer;
        Elf64_Ehdr* Header;
    };
}
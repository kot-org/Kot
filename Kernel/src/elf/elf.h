#pragma once
#include <elf/data.h>
#include <main/kernelInit.h>

namespace ELF{
    /* Function */
    int loadElf(void* buffer, uint8_t ring, struct Parameters* FunctionParameters);
}
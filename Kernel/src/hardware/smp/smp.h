#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../../kernelInit.h"

struct trampolineData{
    uint8_t Status;
    uint64_t GDTPointer;
    uint64_t Paging;
}__attribute__((packed));

extern "C" trampolineData DataTrampoline;
extern "C" void Trampoline();
extern "C" void TrampolineEnd();
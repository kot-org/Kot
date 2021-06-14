#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../../kernelInit.h"

struct trampolineData{
    uint8_t Status;
    uint64_t GDTPointer;
    uint64_t Paging;
    uint64_t Stack;
    uint64_t MainEntry;
}__attribute__((packed));

void LoadCores();

extern "C" trampolineData DataTrampoline;
extern "C" void Trampoline();
extern "C" void TrampolineMain(int cpuID);
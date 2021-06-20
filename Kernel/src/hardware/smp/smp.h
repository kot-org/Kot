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


extern "C" trampolineData DataTrampoline;
extern "C" void Trampoline();
extern "C" void TrampolineMain(int CoreID);

extern "C" bool atomicLock(uint64_t* mutex, uint64_t bit);
extern "C" bool atomicUnlock(uint64_t* mutex, uint64_t bit);
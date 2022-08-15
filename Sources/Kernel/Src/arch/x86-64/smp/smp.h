#pragma once

#include <kot/types.h>
#include <core/core.h>

struct trampolineData{
    uint8_t Status;
    uint64_t Paging;
    uint64_t Stack;
    uint64_t StackScheduler;
    uint64_t MainEntry;
}__attribute__((packed));

extern uint64_t StatusProcessor;

extern "C" trampolineData DataTrampoline;
extern "C" void Trampoline();
extern "C" void TrampolinePaging();
extern "C" void TrampolineMain();
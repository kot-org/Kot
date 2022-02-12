#pragma once
#include "../../../lib/types.h"
#include "../gdt/gdt.h"
#include "../../../memory/memory.h"
#include "../../../lib/stdio.h"
#include "../apic/apic.h"


struct TSS{
    uint32_t Reserved0;

    //RSP
    uint64_t RSP[3];

    uint64_t Reserved1;

    //IST
    uint64_t IST[7];

    uint64_t Reserved2;
    uint16_t Reserved3;

    uint16_t IOPBOffset;
}__attribute__((packed));

void TSSInit();
uint16_t TSSInstall(uint8_t numCPU);

extern "C" void TSSSetStack(uint8_t numCPU, void* stack);
extern "C" uint64_t TSSGetStack(uint8_t numCPU);

extern "C" void LoadTSS(uint8_t tss);
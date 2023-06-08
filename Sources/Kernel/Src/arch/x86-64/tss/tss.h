#pragma once
#include <arch/x86-64/gdt/gdt.h>
#include <arch/x86-64/apic/apic.h>
#include <lib/types.h>
#include <memory/memory.h>
#include <lib/stdio.h>


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

void TSSSetStack(uint8_t numCPU, void* stack);
uint64_t TSSGetStack(uint8_t numCPU);

void TSSSetIST(uint8_t numCPU, uint8_t position, uint64_t value);
uint64_t TSSGetIST(uint8_t numCPU, uint8_t position);


extern "C" void LoadTSS(uint8_t tss);
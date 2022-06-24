#pragma once
#include <kot/types.h>
#include <lib/limits.h>
#include <arch/x86-64/smp/smp.h>

#define GDT_MAX_DESCRIPTORS uint16_Limit / sizeof(GDTEntry)

#define KernelRing  0
#define DriversRing 3
#define DevicesRing 3
#define UserAppRing 3

struct GDTDescriptor{
    uint16_t Size;
    uint64_t Offset;
} __attribute__((packed));

struct AccessByteStruct{
    unsigned IsAccess:1;
    unsigned IsReadableAndWritable:1;
    unsigned DirectionBit:1;
    unsigned IsExecutable:1;
    unsigned DescriptorType:1;
    unsigned Priviledge:2;
    unsigned IsPresent:1;
} __attribute__((packed));

struct OthersStruct{
    uint8_t Limit1:4;
    /*Flags*/
    unsigned Reserved:1;
    unsigned Is64bitsCode:1;
    unsigned Size:1;
    unsigned IsUnitPage4K:1;
} __attribute__((packed));


struct GDTEntry{
    uint16_t Limit0;
    uint16_t Base0;
    uint8_t Base1;
    AccessByteStruct AccessByte;
    OthersStruct Others;
    uint8_t Base2;
}__attribute__((packed));

struct gdtTSSEntry{
    uint16_t Limit0;
    uint16_t Base0;
    uint8_t Base1;
    AccessByteStruct AccessByte;
    uint8_t Limit1;
    uint8_t Base2;
    uint32_t Base3;
    uint32_t Reserved;
}__attribute__((packed));

struct gdtInfoSelectorsRing{
    uint16_t Code;
    uint16_t Data;
};

extern gdtInfoSelectorsRing GDTInfoSelectorsRing[(GDT_MAX_DESCRIPTORS / 2)];

void gdtInit();
uint16_t gdtNullDescriptor();
uint16_t gdtCreateCodeDescriptor(int ring);
uint16_t gdtCreateDataDescriptor(int ring);
void gdtInitCores(uint8_t cpuID);
uint16_t gdtInstallDescriptor(uint64_t base, uint64_t limit, AccessByteStruct access, OthersStruct other);
uint16_t gdtInstallTSS(uint64_t base, uint64_t limit);

uint8_t GetRingPL(uint8_t priviledge);

extern int GDTIndexTable;


extern "C" void LoadGDT(GDTDescriptor* gdtDescriptor);
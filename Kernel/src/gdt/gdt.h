#pragma once
#include <stdint.h>

#define GDT_MAX_DESCRIPTORS  16

#define GDT_DESC_ACCESS 0x01
#define GDT_DESC_READWRITE 0x02
#define GDT_DESC_DC 0x04
#define GDT_DESC_EXECUTABLE 0x08

#define GDT_DESC_CODEDATA 0x10
#define GDT_DESC_DPL 0x60
#define GDT_DESC_PRESENT 0x80
#define GDT_GRAN_LIMITHIMAST 0x0F
#define GDT_GRAN_OS 0x10
#define GDT_GRAN_64BIT 0x20
#define GDT_GRAN_32BIT 0x40
#define GDT_GRAN_4K 0x80


struct GDTDescriptor{
    uint16_t Size;
    uint64_t Offset;
} __attribute__((packed));

struct GDTEntry{
    uint16_t Limit0;
    uint16_t Base0;
    uint8_t Base1;
    uint8_t AccessByte;
    uint8_t Other;
    uint8_t Base2;
}__attribute__((packed));

struct gdtTSSEntry{
    uint16_t Limit0;
    uint16_t Base0;
    uint8_t Base1;
    uint8_t Type;
    uint8_t Limit1;
    uint8_t Base2;
    uint32_t Base3;
    uint32_t Reserved;
} __attribute__((packed));

struct gdtInfoSelectors{
    int KCode;
    int KData;
    int UCode;
    int UData
} __attribute__((packed));

extern gdtInfoSelectors GDTInfoSelectors;

void gdtInit();
int gdtInstallDescriptor(uint64_t base, uint64_t limit, uint8_t access, uint8_t other);
uint16_t gdtInstallTSS(uint64_t base, uint64_t limit);

extern int GDTIndexTable;


extern "C" void LoadGDT(GDTDescriptor* gdtDescriptor);
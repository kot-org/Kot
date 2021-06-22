#include "gdt.h"
#include "../tss/tss.h"


static __attribute__((aligned(0x1000)))GDTEntry GDTEntries[GDT_MAX_DESCRIPTORS];
static GDTDescriptor gdtBaseInfo;

int GDTIndexTable = 0;
int ringGDT = 0; //kernel mode
gdtInfoSelectors GDTInfoSelectors;

const uint8_t BASE_DESC = GDT_DESC_PRESENT | GDT_DESC_READWRITE | GDT_DESC_CODEDATA;
const uint8_t BASE_GRAN = GDT_GRAN_64BIT | GDT_GRAN_4K;

void gdtInit(){
    TSS DefaultTSS;
    gdtBaseInfo.Size = (sizeof(GDTEntry) * GDT_MAX_DESCRIPTORS) - 1; //get the total size where gdt entries are
    gdtBaseInfo.Offset = (uint64_t)&GDTEntries[0]; //get adress of the table where gdt entries are

    setGDTRing(0); //kernel space

    gdtInstallDescriptor(0, 0, 0x00, 0x00); // kernel null
    GDTInfoSelectors.KCode = gdtInstallDescriptor(0, 0, BASE_DESC | GDT_DESC_EXECUTABLE, BASE_GRAN); // kernel code segment
    GDTInfoSelectors.KData = gdtInstallDescriptor(0, 0, BASE_DESC, BASE_GRAN); // kernel data segment

    setGDTRing(3); //user space

    gdtInstallDescriptor(0, 0, 0x00, 0x00); // user null
    GDTInfoSelectors.UData = gdtInstallDescriptor(0, 0, BASE_DESC | GDT_DESC_DPL, BASE_GRAN); // user data segment   
    GDTInfoSelectors.UCode = gdtInstallDescriptor(0, 0, BASE_DESC | GDT_DESC_EXECUTABLE | GDT_DESC_DPL, BASE_GRAN); // user code segment

    setGDTRing(0);

    TSSInit();
    uint16_t TSSlocation = TSSInstall(0);
    DataTrampoline.GDTPointer = (uint64_t)&gdtBaseInfo;
    LoadGDT(&gdtBaseInfo);    

    asm("movw %%ax, %w0\n\t"
                "ltr %%ax" :: "a" (TSSlocation));
}

void gdtInitCores(int cpuID){
    uint16_t TSSlocation = TSSInstall(cpuID);
    LoadGDT(&gdtBaseInfo);
    asm("movw %%ax, %w0\n\t"
                "ltr %%ax" :: "a" (TSSlocation));
}

int gdtInstallDescriptor(uint64_t base, uint64_t limit, uint8_t access, uint8_t flags){
    if(GDTIndexTable >= GDT_MAX_DESCRIPTORS) {
        return 0;
    }

    GDTEntries[GDTIndexTable].Base0 = (uint16_t)(base & 0xFFFF);
    GDTEntries[GDTIndexTable].Base1 = (uint8_t)((base >> 16) & 0xFF);
    GDTEntries[GDTIndexTable].Base2 = (uint8_t)((base >> 24) & 0xFF);
    GDTEntries[GDTIndexTable].Limit0 = (uint16_t)(limit & 0xFFFF);
    GDTEntries[GDTIndexTable].Other = (uint8_t)((limit >> 16) & 0x0F);

    GDTEntries[GDTIndexTable].AccessByte = access;
    GDTEntries[GDTIndexTable].Other |= flags & 0xF0;

    GDTIndexTable++;
    return ((GDTIndexTable - 1) * sizeof(GDTEntry)) | ringGDT;
}

uint16_t gdtInstallTSS(uint64_t base, uint64_t limit){ 
    if(GDTIndexTable >= GDT_MAX_DESCRIPTORS) {
        return 0;
    }

    uint16_t TSSType = GDT_DESC_ACCESS | GDT_DESC_EXECUTABLE | GDT_DESC_PRESENT;
    gdtTSSEntry* gdt_desc = (gdtTSSEntry*)&GDTEntries[GDTIndexTable];

    gdt_desc->Type = (uint16_t)(TSSType & 0x00FF);

    gdt_desc->Base0 = base & 0xFFFF;
    gdt_desc->Base1 = (base & 0xFF0000) >> 16;
    gdt_desc->Base2 = (base & 0xFF000000) >> 24;
    gdt_desc->Base3 = base >> 32;

    gdt_desc->Limit0 = limit & 0xFFFF;
    gdt_desc->Limit1 = (limit & 0xF0000) >> 16;

    gdt_desc->Reserved = 0;

    GDTIndexTable += 2;

    return (uint16_t)((GDTIndexTable - 2) * sizeof(GDTEntry));
}

void setGDTRing(int ring){
    ringGDT = ring;
}
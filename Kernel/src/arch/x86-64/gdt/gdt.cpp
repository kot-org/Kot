#include <arch/x86-64/gdt/gdt.h>
#include <arch/x86-64/tss/tss.h>


static __attribute__((aligned(0x1000)))GDTEntry GDTEntries[GDT_MAX_DESCRIPTORS];
static GDTDescriptor gdtBaseInfo;
static TSS DefaultTSS;
gdtInfoSelectorsRing GDTInfoSelectorsRing[4];
int GDTIndexTable = 0;

void gdtInit(){
    gdtBaseInfo.Size = (sizeof(GDTEntry) * GDT_MAX_DESCRIPTORS) - 1; //get the total size where gdt entries 
    gdtBaseInfo.Offset = (uint64_t)&GDTEntries[0]; //get adress of the table where gdt entries 

    for(int i = 0; i < 4; i++){
        gdtNullDescriptor();
        GDTInfoSelectorsRing[i].Code = gdtCreatCodeDescriptor(i);
        GDTInfoSelectorsRing[i].Data = gdtCreatDataDescriptor(i);
    }
    
    TSSInit();
    uint16_t TSSlocation = TSSInstall(0);
    DataTrampoline.GDTPointer = (uint64_t)&gdtBaseInfo;

    LoadGDT(&gdtBaseInfo);    
    asm("movw %%ax, %w0\n\t" "ltr %%ax" :: "a" (TSSlocation));
}

uint16_t gdtNullDescriptor(){
    OthersStruct flagsStructNull;
    AccessByteStruct accessByteNull;
    flagsStructNull.Is64bitsCode = false;
    flagsStructNull.Reserved = false;
    flagsStructNull.Size = false;
    flagsStructNull.IsUnitPage4K = false;

    accessByteNull.IsAccess = false;
    accessByteNull.IsExecutable = false;
    accessByteNull.IsPresent = false;
    accessByteNull.IsReadableAndWritable = false;
    accessByteNull.Priviledge = 0;
    accessByteNull.DescriptorType = false;
    accessByteNull.DirectionBit = false;

    return gdtInstallDescriptor(0, 0, accessByteNull, flagsStructNull);
}
uint16_t gdtCreatCodeDescriptor(int ring){
    OthersStruct flagsStructCode;
    AccessByteStruct accessByteCode;
    flagsStructCode.Is64bitsCode = false;
    flagsStructCode.Reserved = false;
    flagsStructCode.Size = false;
    flagsStructCode.IsUnitPage4K = false;

    accessByteCode.IsAccess = false;
    accessByteCode.IsExecutable = false;
    accessByteCode.IsPresent = false;
    accessByteCode.IsReadableAndWritable = false;
    accessByteCode.Priviledge = 0;
    accessByteCode.DescriptorType = false;
    accessByteCode.DirectionBit = false;


    flagsStructCode.IsUnitPage4K = true;
    flagsStructCode.Reserved = false;
    flagsStructCode.Is64bitsCode = true;
    accessByteCode.IsPresent = true;
    accessByteCode.DescriptorType = true;
    accessByteCode.IsReadableAndWritable = true;
    accessByteCode.IsExecutable = true;
    accessByteCode.Priviledge = ring;
    return gdtInstallDescriptor(0, 0, accessByteCode, flagsStructCode);
}

uint16_t gdtCreatDataDescriptor(int ring){
    OthersStruct flagsStructData;
    AccessByteStruct accessByteData;
    flagsStructData.Is64bitsCode = false;
    flagsStructData.Reserved = false;
    flagsStructData.Size = false;
    flagsStructData.IsUnitPage4K = false;

    accessByteData.IsAccess = false;
    accessByteData.IsExecutable = false;
    accessByteData.IsPresent = false;
    accessByteData.IsReadableAndWritable = false;
    accessByteData.Priviledge = 0;
    accessByteData.DescriptorType = false;
    accessByteData.DirectionBit = false;


    flagsStructData.IsUnitPage4K = true;
    flagsStructData.Reserved = false;
    flagsStructData.Size = false;

    accessByteData.IsPresent = true;
    accessByteData.Priviledge = ring;
    accessByteData.DescriptorType = true;
    accessByteData.IsReadableAndWritable = true;
    return gdtInstallDescriptor(0, 0, accessByteData, flagsStructData);
}

void gdtInitCores(uint8_t cpuID){
    uint16_t TSSlocation = TSSInstall(cpuID);
    LoadGDT(&gdtBaseInfo);
    asm("movw %%ax, %w0\n\t"
                "ltr %%ax" :: "a" (TSSlocation));
}

uint16_t gdtInstallDescriptor(uint64_t base, uint64_t limit, AccessByteStruct access, OthersStruct flags){
    if(GDTIndexTable >= GDT_MAX_DESCRIPTORS) {
        return 0;
    }

    GDTEntries[GDTIndexTable].Base0 = (uint16_t)(base & 0xFFFF);
    GDTEntries[GDTIndexTable].Base1 = (uint8_t)((base >> 16) & 0xFF);
    GDTEntries[GDTIndexTable].Base2 = (uint8_t)((base >> 24) & 0xFF);
    GDTEntries[GDTIndexTable].Limit0 = (uint16_t)(limit & 0xFFFF);

    GDTEntries[GDTIndexTable].Others = flags;
    GDTEntries[GDTIndexTable].Others.Limit1 = (uint8_t)(limit >> 16);
    
    GDTEntries[GDTIndexTable].AccessByte = access;

    GDTIndexTable++;
    return ((GDTIndexTable - 1) * sizeof(GDTEntry));
}

uint16_t gdtInstallTSS(uint64_t base, uint64_t limit){ 
    if(GDTIndexTable >= GDT_MAX_DESCRIPTORS) {
        return 0;
    }

    gdtTSSEntry* gdt_desc = (gdtTSSEntry*)&GDTEntries[GDTIndexTable];

    gdt_desc->AccessByte.IsPresent = true;
    gdt_desc->AccessByte.IsExecutable = true;
    gdt_desc->AccessByte.IsAccess = true;
    

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
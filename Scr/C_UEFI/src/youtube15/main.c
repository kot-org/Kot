// UEFI From Scratch Series - ThatOSDev ( 2021 )
// https://github.com/ThatOSDev/C_UEFI

#include "efi.h"
#include "ErrorCodes.h"
#include "libs.h"

CHAR16* EFI_MEMORY_TYPE_STRINGS[16] =
{
    L"EfiReservedMemoryType",
    L"EfiLoaderCode",
    L"EfiLoaderData",
    L"EfiBootServicesCode",
    L"EfiBootServicesData",
    L"EfiRuntimeServicesCode",
    L"EfiRuntimeServicesData",
    L"EfiConventionalMemory",
    L"EfiUnusableMemory",
    L"EfiACPIReclaimMemory",
    L"EfiACPIMemoryNVS",
    L"EfiMemoryMappedIO",
    L"EfiMemoryMappedIOPortSpace",
    L"EfiPalCode",
    L"EfiPersistentMemory",
    L"EfiMaxMemoryType"
};

EFI_STATUS efi_main(EFI_HANDLE IH, EFI_SYSTEM_TABLE *ST)
{
    ImageHandle = IH;
    SystemTable = ST;
    
    ResetScreen();
    
    SetColor(EFI_WHITE);
    SetTextPosition(3, 2);
    Print(L"Welcome to the UEFI From Scratch in C Series");

    SetColor(EFI_GREEN);
    SetTextPosition(8, 4);
    Print(L"Hit Any Key to see Graphics and to Load a Graphic Font");
    HitAnyKey();

    InitializeGOP();
    
    InitializeFILESYSTEM();

    TestFont();
    
    SetColor(EFI_GREEN);
    SetTextPosition(2, 20);
    Print(L"Hit Any Key to setup the Memory Map");
    HitAnyKey();
    
    ClearScreen();
    SetColor(EFI_LIGHTGREEN);
    Print(L"******************MEMORY MAP******************\r\n\r\n");
    
    // Setup Memory Map
    UINTN                  MemoryMapSize = 0;
    EFI_MEMORY_DESCRIPTOR  *MemoryMap;
    UINTN                  MapKey;
    UINTN                  DescriptorSize;
    UINT32                 DescriptorVersion;
    
    SystemTable->BootServices->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);
    MemoryMapSize += 2 * DescriptorSize;
    SystemTable->BootServices->AllocatePool(EfiLoaderData, MemoryMapSize, (void **)&MemoryMap);
    SystemTable->BootServices->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);

    long long MapEntries = MemoryMapSize / DescriptorSize;
    int t = (MapEntries / 2) + 1;
    UINT64 TotalRam = 0;
    for(long long i = 0; i < t; i++)
    {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((UINT64)MemoryMap + (i * DescriptorSize));
        SetColor(EFI_LIGHTCYAN);
        Print(EFI_MEMORY_TYPE_STRINGS[desc->Type]);
        SetColor(EFI_BROWN);
        Print(L" ");
        CHAR16 buf[8];
        itoa(desc->NumberOfPages * 4096 / 1024, buf, 10);
        Print(buf);
        SetColor(EFI_LIGHTGRAY);
        Print(L" KB\r\n");
        TotalRam += desc->NumberOfPages * 4096 / 1024;
    }
    
    int pos = 2;
    for(long long i = t; i < MapEntries; i++)
    {
        SetTextPosition(40, pos);
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((UINT64)MemoryMap + (i * DescriptorSize));
        SetColor(EFI_LIGHTCYAN);
        Print(EFI_MEMORY_TYPE_STRINGS[desc->Type]);
        SetColor(EFI_BROWN);
        Print(L" ");
        CHAR16 buf[8];
        itoa(desc->NumberOfPages * 4096 / 1024, buf, 10);
        Print(buf);
        SetColor(EFI_LIGHTGRAY);
        Print(L" KB");
        TotalRam += desc->NumberOfPages * 4096 / 1024;
        pos++;
    }
    
    SetColor(EFI_LIGHTGREEN);
    SetTextPosition(0, 20);
    Print(L"Total System Ram : ");
    SetColor(EFI_CYAN);
    CHAR16 tr[8];
    itoa(TotalRam, tr, 10);
    Print(tr);
    SetColor(EFI_LIGHTGRAY);
    Print(L" KBytes");
    
    SetColor(EFI_GREEN);
    SetTextPosition(10, 22);
    Print(L"Hit Any Key to Cold-Reboot your computer");
    HitAnyKey();
    
    COLD_REBOOT();
   
	return 0;
}

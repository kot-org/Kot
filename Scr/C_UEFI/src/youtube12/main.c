// UEFI From Scratch Series - ThatOSDev ( 2021 )
// https://github.com/ThatOSDev/C_UEFI

#include "efi.h"
#include "ErrorCodes.h"
#include "libs.h"

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
    Print(L"Hit Any Key to see Graphics");
    
    HitAnyKey();

    InitializeGOP();
    
    InitializeFILESYSTEM();
    
    EFI_FILE_PROTOCOL* file_handle = openFile(L"texttest.txt");
    if(file_handle != NULL)
    {
        void* OSBuffer_Handle;
        
        SetColor(EFI_BROWN);
        Print(L"AllocatingPool ... ");
        Status = SystemTable->BootServices->AllocatePool(EfiLoaderData, 0x00100000, (void**)&OSBuffer_Handle);
        SetColor(EFI_CYAN);
        Print(CheckStandardEFIError(Status));
    
        SetColor(EFI_BROWN);
        Print(L"Reading File ... ");
        Status = file_handle->Read(file_handle, (UINTN*)0x00100000, OSBuffer_Handle);
        SetColor(EFI_CYAN);
        Print(CheckStandardEFIError(Status));

        closeFile(file_handle);
    }
    
    // OSBuffer_Handle This is our new File Handle. We can access the file from here.
    
    SetColor(EFI_GREEN);
    SetTextPosition(2, 20);
    Print(L"Hit Any Key to Cold-Reboot your computer");
    
    HitAnyKey();
    
    COLD_REBOOT();
   
	return 0;
}

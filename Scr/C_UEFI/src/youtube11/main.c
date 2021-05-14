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
    
    closeFile(file_handle);
    
    
    SetColor(EFI_GREEN);
    SetTextPosition(2, 20);
    Print(L"Hit Any Key to Cold-Reboot your computer");
    
    HitAnyKey();
    
    COLD_REBOOT();
   
	return 0;
}

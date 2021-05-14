// UEFI From Scratch Series - ThatOSDev ( 2021 )
// https://github.com/ThatOSDev/C_UEFI

#include "efi.h"
#include "ErrorCodes.h"
#include "libs.h"

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *ST)
{
    SystemTable = ST;
    
    ClearScreen();
    
    SetColor(EFI_WHITE);
    
    Print(L"Welcome to the UEFI From Scratch in C Series\r\n\r\n");
    
    SetColor(EFI_YELLOW);

    Print(L"We have a libs file.\r\n\r\n");
    
    SetColor(EFI_GREEN);
    
    Print(L"\r\nHit Any Key\r\n");
    
    HitAnyKey();
   
	return 0;
}

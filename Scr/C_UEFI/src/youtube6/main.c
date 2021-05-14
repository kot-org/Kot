// UEFI From Scratch Series - ThatOSDev ( 2021 )
// https://github.com/ThatOSDev/C_UEFI

#include "efi.h"
#include "ErrorCodes.h"
#include "libs.h"

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *ST)
{
    SystemTable = ST;
    
    ResetScreen();
    
    SetColor(EFI_WHITE);
    SetTextPosition(3, 2);
    Print(L"Welcome to the UEFI From Scratch in C Series");
    
    SetColor(EFI_YELLOW);
    SetTextPosition(3, 5);
    Print(L"We have Simple-Text Cursor Positions.");
    
    SetColor(EFI_GREEN);
    SetTextPosition(8, 8);
    Print(L"Hit Any Key to Clear the Screen");
    
    HitAnyKey();
    
    ClearScreen();

    SetColor(EFI_CYAN);
    SetTextPosition(2, 1);
    Print(L"The Screen has been cleared !");
    
    SetColor(EFI_GREEN);
    SetTextPosition(2, 4);
    Print(L"Hit Any Key");
    
    HitAnyKey();
   
	return 0;
}

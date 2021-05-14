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

    SetColor(EFI_GREEN);
    SetTextPosition(8, 4);
    Print(L"Hit Any Key to see Graphics");
    
    HitAnyKey();

    Print(L"\r\n\r\nLoading Graphics Output Protocol ... ");
    EFI_STATUS Status = SystemTable->BootServices->LocateProtocol(&EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID, 0, (void**)&gop);
    Print(CheckStandardEFIError(Status));

    if(Status == EFI_SUCCESS)
    {
        CreateFilledBox(50, 50, 100, 200, ORANGE);
        CreateFilledBox(60, 60, 80, 30, RED);
        SetColor(EFI_YELLOW);
        SetTextPosition(3, 8);
        Print(L"We have Graphics !!");
    }
    
    SetColor(EFI_GREEN);
    SetTextPosition(2, 10);
    Print(L"Hit Any Key");
    
    HitAnyKey();
   
	return 0;
}

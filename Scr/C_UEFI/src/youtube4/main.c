// UEFI From Scratch Series - ThatOSDev ( 2021 )
// https://github.com/ThatOSDev/C_UEFI

#include "efi.h"
#include "ErrorCodes.h"

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    SystemTable->ConOut->Reset(SystemTable->ConOut, 1);
    
    SystemTable->ConOut->SetAttribute(SystemTable->ConOut, EFI_WHITE);

    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Welcome to the UEFI From Scratch in C Series\r\n\r\n");
    
    SystemTable->ConOut->SetAttribute(SystemTable->ConOut, EFI_YELLOW);

    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"We now have Errors Checking !!!\r\n\r\n");
    
    SystemTable->ConOut->SetAttribute(SystemTable->ConOut, EFI_LIGHTGREEN);

    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Flushing Keyboard ... ");
    
    EFI_STATUS results = SystemTable->ConIn->Reset(SystemTable->ConIn, 1);
    
    SystemTable->ConOut->SetAttribute(SystemTable->ConOut, EFI_CYAN);
    
    SystemTable->ConOut->OutputString(SystemTable->ConOut, CheckStandardEFIError(results));
    
    SystemTable->ConOut->SetAttribute(SystemTable->ConOut, EFI_GREEN);
    
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"\r\nHit Any Key\r\n");

    EFI_INPUT_KEY Key;

	while((SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key)) == EFI_NOT_READY);
   
	return 0;
}

// UEFI From Scratch Series - ThatOSDev ( 2021 )
// https://github.com/ThatOSDev/C_UEFI

#include "efi.h"

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    SystemTable->ConOut->Reset(SystemTable->ConOut, 1);

    SystemTable->ConOut->SetAttribute(SystemTable->ConOut, EFI_LIGHTGREEN);
    
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Hit Any Key\r\n");
    
    SystemTable->ConIn->Reset(SystemTable->ConIn, 1);

    EFI_INPUT_KEY Key;

	while((SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key)) == EFI_NOT_READY);
   
	return 0;
}

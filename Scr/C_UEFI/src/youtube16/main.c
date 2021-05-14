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
    Print(L"Hit Any Key to see Graphics and to Load a Graphic Font");
    HitAnyKey();

    InitializeGOP();
    
    InitializeFILESYSTEM();

    TestFont();
    
    RunMemMap();
    
    SetColor(EFI_GREEN);
    SetTextPosition(10, 22);
    Print(L"Hit Any Key to get value from loaded file");
    HitAnyKey();
    
    ClearScreen();
    SetColor(EFI_LIGHTGREEN);
    Print(L"Loaded File... \r\n\r\n");
    
        // Kernel stuff here
    
    void* OSKernelBuffer;

    EFI_FILE_PROTOCOL* efikernel = openFile(L"kernel.bin");

    UINT64 fsize = 0x00100000;

    Status = SystemTable->BootServices->AllocatePool(EfiLoaderData, fsize, (void**)&OSKernelBuffer);
    Print(L"AllocatePool OSKernelBuffer");
    Print(CheckStandardEFIError(Status));

    efikernel->SetPosition(efikernel, 0);
    
    efikernel->Read(efikernel, &fsize, OSKernelBuffer);
    SetColor(EFI_GREEN);
    Print(L"\r\nRead OSKernelBuffer");
    Print(CheckStandardEFIError(Status));

    SetColor(EFI_LIGHTCYAN);    
    Print(L"\r\nFile Signature\r\n");
    SetColor(EFI_LIGHTRED);    
    UINT8* test = (UINT8*)OSKernelBuffer;

    for(int m = 0; m < 5; m++)
    {
        int j = *test;
        UINT16 s[2];
        itoa(j, s, HEX);
        Print(s);
        Print(L"  ");
        test++;
    }

    Print(L"\r\n\r\n");
    
    efikernel->SetPosition(efikernel, 262);
    
    efikernel->Read(efikernel, &fsize, OSKernelBuffer);
    SetColor(EFI_GREEN);
    Print(L"Read OSKernelBuffer");
    Print(CheckStandardEFIError(Status));
    
    closeFile(efikernel);
    
    SetColor(EFI_LIGHTCYAN);    
    Print(L"\r\nEntry Point\r\n");
    SetColor(EFI_YELLOW);
    UINT8* test2 = (UINT8*)OSKernelBuffer;
    for(int m = 0; m < 4; m++)
    {
        int j = *test2;
        UINT16 s[2];
        itoa(j, s, HEX);
        Print(s);
        Print(L"  ");
        test2++;
    }

    SetColor(EFI_LIGHTCYAN);
    int (*KernelBinFile)() = ((__attribute__((ms_abi)) int (*)() ) (UINT8*)OSKernelBuffer);      //&test[0]);
    int g = KernelBinFile();
    
    UINT16 tmp[8];
    itoa(g, tmp, 10);
    Print(L"\r\n\r\nThe returned number is : ");
    SetColor(EFI_LIGHTMAGENTA);
    Print(tmp);
    
    SetColor(EFI_GREEN);
    SetTextPosition(10, 24);
    Print(L"Hit Any Key to exit");
    HitAnyKey();
    
    COLD_REBOOT();
   
	return 0;
}

// UEFI From Scratch Series - ThatOSDev ( 2021 )
// https://github.com/ThatOSDev/C_UEFI

#include "efi.h"
#include "ErrorCodes.h"
#include "libs.h"

typedef struct PSF1_FONT
{
    UINT8        Signature[2];
    UINT8        mode;
    UINT8        charsize;
    char*        glyphBuffer[256];
} PSF1_FONT;

PSF1_FONT* LinuxFont;

void GraphicChar(char chr, unsigned int xOff, unsigned int yOff)
{
    char* fontPtr = (char*)LinuxFont->glyphBuffer + (chr * LinuxFont->charsize) - 3;
    for (unsigned long y = yOff; y < yOff + 16; y++){
        for (unsigned long x = xOff; x < xOff+8; x++){
            if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0){
                   gop->Blt(gop, &GraphicsColor, 0, 0, 0, x, y, 1, 1, 0);
                }
        }
        fontPtr++;
    }
}

typedef struct GCP{
    unsigned int X;
    unsigned int Y;
} GCP;

GCP GraphicCursorPosition;

void SetGraphicCursorPosition(unsigned int x, unsigned int y)
{
    GraphicCursorPosition.X = x;
    GraphicCursorPosition.Y = y;
}

void Printgraphictext(const char* str)
{
    char* chr = (char*)str;
    while(*chr != 0){
        GraphicChar(*chr, GraphicCursorPosition.X, GraphicCursorPosition.Y);
        GraphicCursorPosition.X+=8;
        if(GraphicCursorPosition.X + 8 > gop->Mode->Info->HorizontalResolution)
        {
            GraphicCursorPosition.X = 0;
            GraphicCursorPosition.Y += 16;
        }
        chr++;
    }
}

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

    EFI_FILE_PROTOCOL* font = openFile(L"zap-light16.psf");

    UINTN lfsize = sizeof(PSF1_FONT);
    SetColor(EFI_BROWN);
    Print(L"AllocatingPool ... ");
    Status = SystemTable->BootServices->AllocatePool(EfiLoaderData, lfsize, (void**)&LinuxFont);
    SetColor(EFI_CYAN);
    Print(CheckStandardEFIError(Status));

    SetColor(EFI_BROWN);
    Print(L"Reading File ... ");
    Status = font->Read(font, &lfsize, LinuxFont);
    SetColor(EFI_CYAN);
    Print(CheckStandardEFIError(Status));
        
    closeFile(font);
    
    if(LinuxFont == NULL)
    {
        Print(L"Font Not Found\r\n");
    } else {
        Print(L"Font Found.");
        SetGraphicsColor(ORANGE);
        SetGraphicCursorPosition(250, 53);
        Printgraphictext("This is a Graphics text-string");
    }
    
    SetColor(EFI_GREEN);
    SetTextPosition(2, 20);
    Print(L"Hit Any Key to Cold-Reboot your computer");
    
    HitAnyKey();
    
    COLD_REBOOT();
   
	return 0;
}

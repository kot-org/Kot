#ifndef LIBS_H
#define LIBS_H

             // ARGB             
#define ORANGE 0xffffa500
#define CYAN   0xff00ffff
#define RED    0xffff0000
#define GREEN  0xff00ff00
#define BLUE   0xff0000ff
#define GRAY   0xff888888
#define WHITE  0xffffffff
#define BLACK  0xff000000

EFI_SYSTEM_TABLE* SystemTable;
EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;

void SetTextPosition(UINT32 Col, UINT32 Row)
{
    SystemTable->ConOut->SetCursorPosition(SystemTable->ConOut, Col, Row);
}

void ResetScreen()
{
    SystemTable->ConOut->Reset(SystemTable->ConOut, 1);
}

void ClearScreen()
{
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
}

void SetColor(UINTN Attribute)
{
    SystemTable->ConOut->SetAttribute(SystemTable->ConOut, Attribute);
}

void Print(CHAR16* str)
{
    SystemTable->ConOut->OutputString(SystemTable->ConOut, str);
}

void HitAnyKey()
{
    SystemTable->ConIn->Reset(SystemTable->ConIn, 1);

    EFI_INPUT_KEY Key;
	while((SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key)) == EFI_NOT_READY);
}

void CreateFilledBox(UINT32 xPos, UINT32 yPos, UINT32 w, UINT32 h, UINT32 color)
{
    // This functions puts a color filled box on the screen
    UINT32 BBP = 4;
    if(xPos < 0){xPos = 0;}
    if(yPos < 0){yPos = 0;}
    if(w < 1){w = 1;}
    if(h < 1){h = 1;}
    UINT32 x;
    UINT32 y      = yPos;
    UINT32 width  = (xPos + w) * BBP;
    UINT32 height = yPos + h;

    for(y = yPos; y <= height; y++)
    {
        for(x = xPos * BBP; x <= width; x+=BBP)
        {
            *(UINT32*)(x + (y * gop->Mode->Info->PixelsPerScanLine * BBP) + (void*)gop->Mode->FrameBufferBase) = color;
        }
    }
}

#endif // LIBS_H
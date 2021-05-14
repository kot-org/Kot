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
EFI_GRAPHICS_OUTPUT_BLT_PIXEL GraphicsColor;

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

void SetGraphicsColor(UINT32 color)
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL GColor;
    GColor.Reserved = color >> 24;
    GColor.Red      = color >> 16;
    GColor.Green    = color >> 8;
    GColor.Blue     = color;
    GraphicsColor = GColor;
}

void SetPixel(UINT32 xPos, UINT32 yPos)
{
    gop->Blt(gop, &GraphicsColor, 0, 0, 0, xPos, yPos, 1, 1, 0);
}

void CreateFilledBox(UINT32 xPos, UINT32 yPos, UINT32 w, UINT32 h, UINT32 color)
{
    SetGraphicsColor(color);
    gop->Blt(gop, &GraphicsColor, 0, 0, 0, xPos, yPos, w, h, 0);
}

#endif // LIBS_H
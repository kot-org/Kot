#ifndef LIBS_H
#define LIBS_H

EFI_SYSTEM_TABLE* SystemTable;

void ClearScreen()
{
    SystemTable->ConOut->Reset(SystemTable->ConOut, 1);
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

#endif // LIBS_H
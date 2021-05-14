#pragma once
#include <efi.h>
#include <efilib.h>
#include <elf.h>
#include <stddef.h>
#include "clib.h"

void PowerDown()
{
	ST->RuntimeServices->ResetSystem(EfiResetShutdown, 0, 0, NULL);
}

void PowerRestart()
{
	ST->RuntimeServices->ResetSystem(EfiResetWarm, 0, 0, NULL);
}

void ShowCursor(BOOLEAN visible)
{
	ST->ConOut->EnableCursor(ST->ConOut,visible);
}

void SetColour(UINTN colour)
{
	ST->ConOut->SetAttribute(ST->ConOut, colour);
}

void InitUEFI(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	ST = SystemTable; //Ceva initializare
	InitializeLib(ImageHandle, SystemTable);
	SystemTable->BootServices->SetWatchdogTimer(0, 0, 0, NULL); //dezactivare blocare dupa 5 minuturi
}

void SetCursorPos(UINTN x,UINTN y)
{
	ST->ConOut->SetCursorPosition(ST->ConOut,x,y);
}

void ClearScreen()
{
	ST->ConOut->ClearScreen(ST->ConOut);
	SetCursorPos(0,0);
}

wchar_t *CSTRTOWCHAR(char *cstr, EFI_SYSTEM_TABLE *SystemTable)
{
	wchar_t *toret;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(wchar_t) * strlen(cstr), (void **)&toret);
	for (int i = 0; cstr[i] != 0; i++)
	{
		toret[i] = (wchar_t)cstr[i];
		toret[i + 1] = 0;
	}
	return toret;
}

EFI_INPUT_KEY GetKey(EFI_SYSTEM_TABLE *SystemTable)
{
	SystemTable->ConIn->Reset(SystemTable->ConIn, 1);
	EFI_INPUT_KEY Key;
	while((SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key)) == EFI_NOT_READY);
	return Key;
}

EFI_FILE *ReadFile(EFI_FILE *Directory, CHAR16 *Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_FILE *LoadedFile;

	EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
	SystemTable->BootServices->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (void **)&LoadedImage);

	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem;
	SystemTable->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void **)&FileSystem);

	if (Directory == NULL)
	{
		FileSystem->OpenVolume(FileSystem, &Directory);
	}

	EFI_STATUS s = Directory->Open(Directory, &LoadedFile, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	if (s != EFI_SUCCESS)
	{
		return NULL;
	}
	return LoadedFile;
}


char *ReadText(wchar_t *filename, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_FILE *f = ReadFile(NULL, filename, ImageHandle, SystemTable);
	if (f == NULL)
		return 0;
	char *buf;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(char) * 2048, (void **)&buf);
	UINTN size = sizeof(char) * 2048;
	buf = "";
	f->Read(f, &size, buf);
	buf[2048] = 0;
	return buf;
}
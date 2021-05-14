#include "def.h"

framebuffer frambuf;

void TriggerError(wchar_t *errstr)
{
	ClearScreen();
	Print(errstr);
	while (1);
}

framebuffer *InitGOP()
{
	EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
	EFI_STATUS stat;

	stat = BS->LocateProtocol(&gopGuid, ((void *)0), (void **)&gop);
	if (EFI_ERROR(stat))
	{
		ClearScreen();
		Print(L"Cannot init GOP!\n\r");
		while (1);
	}
	else
	{
		frambuf.BaseAddr = (void *)gop->Mode->FrameBufferBase;
		frambuf.BufferSize = gop->Mode->FrameBufferSize;
		frambuf.Width = gop->Mode->Info->HorizontalResolution;
		frambuf.Height = gop->Mode->Info->VerticalResolution;
		frambuf.PixelPerScanLine = gop->Mode->Info->PixelsPerScanLine;
		return &frambuf;
	}
}

PSF1_FONT *LoadFont(EFI_FILE *Directory, CHAR16 *Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_FILE *font = ReadFile(Directory, Path, ImageHandle, SystemTable);
	if (font == NULL)
		return NULL;

	PSF1_HEADER *fontHeader;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1_HEADER), (void **)&fontHeader);
	UINTN size = sizeof(PSF1_HEADER);
	font->Read(font, &size, fontHeader);

	if (fontHeader->magic[0] != PSF1_MAGIC0 || fontHeader->magic[1] != PSF1_MAGIC1)
	{
		return NULL;
	}

	UINTN glyphBufferSize = fontHeader->charsize * 256;
	if (fontHeader->mode == 1)
	{ //512 glyph mode
		glyphBufferSize = fontHeader->charsize * 512;
	}

	void *glyphBuffer;
	{
		font->SetPosition(font, sizeof(PSF1_HEADER));
		SystemTable->BootServices->AllocatePool(EfiLoaderData, glyphBufferSize, (void **)&glyphBuffer);
		font->Read(font, &glyphBufferSize, glyphBuffer);
	}

	PSF1_FONT *finishedFont;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1_FONT), (void **)&finishedFont);
	finishedFont->psf1_Header = fontHeader;
	finishedFont->glyphBuffer = glyphBuffer;
	return finishedFont;
}

void RunKernel(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	Print(L"Starting LLOS...");
	EFI_FILE* llosFolder = ReadFile(NULL,L"LLOS",ImageHandle,SystemTable);
	if(llosFolder == NULL)
		TriggerError(L"Cannot find \"LLOS\" folder!");

	EFI_FILE *monkernel = ReadFile(llosFolder, L"kernel.llexec", ImageHandle, SystemTable);
	if (monkernel == NULL)
		TriggerError(L"Cannot find \"kernel.llexec\"!");

	Elf64_Ehdr header;
	{
		UINTN FileInfoSize;
		EFI_FILE_INFO *FileInfo;
		monkernel->GetInfo(monkernel, &gEfiFileInfoGuid, &FileInfoSize, NULL);
		SystemTable->BootServices->AllocatePool(EfiLoaderData, FileInfoSize, (void **)&FileInfo);
		monkernel->GetInfo(monkernel, &gEfiFileInfoGuid, &FileInfoSize, (void **)&FileInfo);

		UINTN size = sizeof(header);
		monkernel->Read(monkernel, &size, &header);
	}

	if (memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 || header.e_ident[EI_CLASS] != ELFCLASS64 || header.e_ident[EI_DATA] != ELFDATA2LSB || header.e_type != ET_EXEC || header.e_machine != EM_X86_64 || header.e_version != EV_CURRENT)
		TriggerError(L"Cannot verify the kernel!");

	Elf64_Phdr *phdrs;
	{
		monkernel->SetPosition(monkernel, header.e_phoff);
		UINTN size = header.e_phnum * header.e_phentsize;
		SystemTable->BootServices->AllocatePool(EfiLoaderData, size, (void **)&phdrs);
		monkernel->Read(monkernel, &size, phdrs);
	}

	for (Elf64_Phdr *phdr = phdrs; (char *)phdr < (char *)phdrs + header.e_phnum * header.e_phentsize; phdr = (Elf64_Phdr *)((char *)phdr + header.e_phentsize))
	{
		switch (phdr->p_type)
		{
		case PT_LOAD:
		{
			int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
			Elf64_Addr segment = phdr->p_paddr;
			SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);

			monkernel->SetPosition(monkernel, phdr->p_offset);
			UINTN size = phdr->p_filesz;
			monkernel->Read(monkernel, &size, (void *)segment);
			break;
		}
		}
	}

	PSF1_FONT *newFont = LoadFont(llosFolder, L"font.psf", ImageHandle, SystemTable);
	if (newFont == NULL)
		TriggerError(L"Cannot load \"font.psf\"!");

	InitGOP();

	EFI_MEMORY_DESCRIPTOR *Map = NULL;
	UINTN MapSize, MapKey;
	UINTN DescriptorSize;
	UINT32 DescriptorVersion;
	{
		SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
		SystemTable->BootServices->AllocatePool(EfiLoaderData, MapSize, (void **)&Map);
		for(int i = 0;i<8;i++)
			SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
	}

	EFI_CONFIGURATION_TABLE* configTable = SystemTable->ConfigurationTable;
	void* RSDP = NULL;
	EFI_GUID ACPITABLEGUID = ACPI_20_TABLE_GUID;

	for(UINTN i = 0;i < SystemTable->NumberOfTableEntries;i++) {
		if(CompareGuid(&configTable[i].VendorGuid, &ACPITABLEGUID)) {
			if(strcmp((CHAR8*)"RSD PTR ", (CHAR8*)configTable->VendorTable,8)) {
				RSDP = (void*)configTable->VendorTable;
			}
		}
		configTable++;
	}

	int (*EntryPoint)(BootInfo *) = ((__attribute__((sysv_abi)) int (*)(BootInfo *))header.e_entry);

	UEFIFirmware f;
	f.Vendor = SystemTable->FirmwareVendor;
	f.Version = SystemTable->FirmwareRevision;

	Power p;
	p.PowerOff = PowerDown;
	p.Restart = PowerRestart;

	BootInfo info;
	info.framebuf = &frambuf;
	info.font = newFont;
	info.pwr = &p;
	info.mMap = Map;
	info.mMapSize = MapSize;
	info.mMapDescSize = DescriptorSize;
	info.firm = &f;
	info.Key = 0xFFFFFF/0x800;
	info.RSDP = RSDP;

	SystemTable->BootServices->ExitBootServices(ImageHandle, MapKey);

	EntryPoint(&info);

	TriggerError(L"Cannot run the kernel!");
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	InitUEFI(ImageHandle, SystemTable);
	RunKernel(ImageHandle,SystemTable);
	
	return 1;
}

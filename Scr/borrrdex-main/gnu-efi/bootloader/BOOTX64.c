#include <efi.h>
#include <efilib.h>
#include <elf.h>
#include <stddef.h>

typedef struct {
	void* baseAddress;
	size_t bufferSize;
	unsigned width;
	unsigned height;
	unsigned pixelsPerScanline;
} Framebuffer;

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

typedef struct {
	unsigned char magic[2];
	unsigned char mode;
	unsigned char charsize;
} PSF1_HEADER;

typedef struct {
	PSF1_HEADER* psf1_header;
	void* glyphBuffer;
} PSF1_FONT;

EFI_FILE* LoadFile(EFI_FILE* directory, CHAR16* path, EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* systemTable) {
	EFI_FILE* loadedFile;

	EFI_LOADED_IMAGE_PROTOCOL* loadedImage;
	systemTable->BootServices->HandleProtocol(imageHandle, &gEfiLoadedImageProtocolGuid, (void **)&loadedImage);

	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fileSystem;
	systemTable->BootServices->HandleProtocol(loadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void **)&fileSystem);

	if(!directory) {
		fileSystem->OpenVolume(fileSystem, &directory);
	}

	EFI_STATUS s = directory->Open(directory, &loadedFile, path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	if(s != EFI_SUCCESS) {
		return NULL;
	}

	return loadedFile;
}

PSF1_FONT* LoadPSF1Font(EFI_FILE* directory, CHAR16* path, EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* systemTable) {
	EFI_FILE* font = LoadFile(directory, path, imageHandle, systemTable);
	if(!font) {
		return NULL;
	}

	PSF1_HEADER* fontHeader;
	systemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1_HEADER), (void **)&fontHeader);
	UINTN size = sizeof(PSF1_HEADER);
	font->Read(font, &size, fontHeader);

	if(fontHeader->magic[0] != PSF1_MAGIC0 || fontHeader->magic[1] != PSF1_MAGIC1) {
		return NULL;
	}

	UINTN glyphBufferSize = fontHeader->charsize * 256;
	if(fontHeader->mode == 1) {
		glyphBufferSize = fontHeader->charsize * 512;
	}

	void* glyphBuffer;
	font->SetPosition(font, sizeof(PSF1_HEADER));
	systemTable->BootServices->AllocatePool(EfiLoaderData, glyphBufferSize, &glyphBuffer);
	font->Read(font, &glyphBufferSize, glyphBuffer);

	PSF1_FONT* finishedFont;
	systemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1_FONT), (void **)&finishedFont);
	finishedFont->psf1_header = fontHeader;
	finishedFont->glyphBuffer = glyphBuffer;

	return finishedFont;
}

Framebuffer framebuffer;
Framebuffer* InitializeGOP() {
	EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
	EFI_STATUS status;

	status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void **)&gop);
	if(EFI_ERROR(status)) {
		Print(L"Unable to locate GOP\r\n");
		return NULL;
	}

	framebuffer.baseAddress = (void *)gop->Mode->FrameBufferBase;
	framebuffer.bufferSize = gop->Mode->FrameBufferSize;
	framebuffer.width = gop->Mode->Info->HorizontalResolution;
	framebuffer.height = gop->Mode->Info->VerticalResolution;
	framebuffer.pixelsPerScanline = gop->Mode->Info->PixelsPerScanLine;

	return &framebuffer;
}

int memcmp(const void* aptr, const void* bptr, size_t n) {
	const unsigned char* a = aptr, *b = bptr;
	for(size_t i = 0; i < n; i++) {
		if(a[i] != b[i]) {
			return b[i] - a[i];
		}
	}

	return 0;
}

typedef struct {
	Framebuffer* framebuffer;
	PSF1_FONT* psf1_font;
	EFI_MEMORY_DESCRIPTOR* mMap;
	UINTN mMapSize;
	UINTN mMapDescriptorSize;
	void* rsdp;
} BootInfo;

void* FindRSDP(EFI_SYSTEM_TABLE* st)
{
	EFI_GUID acpi20 = ACPI_20_TABLE_GUID;
	for(UINTN i = 0; i < st->NumberOfTableEntries; i++) {
		EFI_CONFIGURATION_TABLE t = st->ConfigurationTable[i];
		if(CompareGuid(&acpi20, &t.VendorGuid) == 0) {
			return t.VendorTable;
		}
	}

	return NULL;
}

EFI_STATUS efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
	InitializeLib(ImageHandle, SystemTable);
	
	EFI_FILE* kernel = LoadFile(NULL, L"kernel.elf", ImageHandle, SystemTable);
	if(!kernel) {
		Print(L"Could not load kernel\r\n");
		return EFI_LOAD_ERROR;
	}

	Elf64_Ehdr header;
	UINTN fileInfoSize;
	EFI_FILE_INFO* fileInfo;

	kernel->GetInfo(kernel, &gEfiFileInfoGuid, &fileInfoSize, NULL);
	SystemTable->BootServices->AllocatePool(EfiLoaderData, fileInfoSize, (void **)&fileInfo);
	kernel->GetInfo(kernel, &gEfiFileInfoGuid, &fileInfoSize, (void **)&fileInfo);
	
	UINTN size = sizeof(header);
	kernel->Read(kernel, &size, &header);

	if(
		memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) ||
		header.e_ident[EI_CLASS] != ELFCLASS64 ||
		header.e_ident[EI_DATA] != ELFDATA2LSB ||
		header.e_type != ET_EXEC ||
		header.e_machine != EM_X86_64 ||
		header.e_version != EV_CURRENT
	) {
		Print(L"Kernel format is bad\r\n");
		return EFI_UNSUPPORTED;
	}

	Elf64_Phdr* phdrs;
	kernel->SetPosition(kernel, header.e_phoff);
	size = header.e_phnum * header.e_phentsize;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, size, (void **)&phdrs);
	kernel->Read(kernel, &size, phdrs);

	for(
		Elf64_Phdr* phdr = phdrs;
		(char *)phdr < (char *)phdrs + header.e_phnum * header.e_phentsize;
		phdr = (Elf64_Phdr*)((char *)phdr + header.e_phentsize)
	) {
		switch (phdr->p_type) {
			case PT_LOAD:
			{
				int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
				Elf64_Addr segment = phdr->p_vaddr;
				EFI_STATUS status = SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);
				if(status != EFI_SUCCESS) {
					Print(L"Failed to allocate memory for kernel (%d)!", status);
					return status;
				}

				kernel->SetPosition(kernel, phdr->p_offset);
				UINTN size = phdr->p_filesz;
				kernel->Read(kernel, &size, (void *)segment);
				break;
			}
		}
	}

	PSF1_FONT* newFont = LoadPSF1Font(NULL, L"zap-light16.psf", ImageHandle, SystemTable);
	if(!newFont) {
		Print(L"Font is not valid or is not found\r\n");
		return EFI_LOAD_ERROR;
	}

	Framebuffer* newBuffer = InitializeGOP();

	EFI_MEMORY_DESCRIPTOR* map = NULL;
	UINTN mapSize = 0, mapKey = 0;
	UINTN descriptorSize = 0;
	UINT32 descriptorVersion = 0;

	SystemTable->BootServices->GetMemoryMap(&mapSize, NULL, &mapKey, &descriptorSize, &descriptorVersion);
	SystemTable->BootServices->AllocatePool(EfiLoaderData, mapSize, (void **)&map);
	EFI_STATUS mapStatus = SystemTable->BootServices->GetMemoryMap(&mapSize, map, &mapKey, &descriptorSize, &descriptorVersion);

	int attempts = 0;
	while(mapStatus != EFI_SUCCESS) {
		if(attempts++ == 10) {
			Print(L"Failed to determine memory map size, giving up on OS load...");
			return EFI_OUT_OF_RESOURCES;
		}

		// The allocation a couple lines up might have changed the memory map so that it is bigger than what we originally
		// got as the result.  This simple method will increase in increments of descriptorSize until it is big enough
		// or until we start getting silly
		SystemTable->BootServices->FreePool(map);
		mapSize += descriptorSize;
		SystemTable->BootServices->AllocatePool(EfiLoaderData, mapSize, (void **)&map);
		mapStatus = SystemTable->BootServices->GetMemoryMap(&mapSize, map, &mapKey, &descriptorSize, &descriptorVersion);
	}

	void(*KernelStart)(BootInfo*) = ((__attribute((sysv_abi)) void(*)(BootInfo*)) header.e_entry);

	BootInfo bootInfo = {
		newBuffer,
		newFont,
		map,
		mapSize,
		descriptorSize,
		FindRSDP(SystemTable)
	};

	SystemTable->BootServices->ExitBootServices(ImageHandle, mapKey);

	KernelStart(&bootInfo);

	return EFI_SUCCESS; // Exit the UEFI application
}

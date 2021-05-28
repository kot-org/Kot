#include "types.h"
#include "efi.h"
#include "bootinfo.h"
#include "errorCodes.h"
#include "lib.h"
#include "elf.h"

EFI_STATUS efi_main(EFI_HANDLE IH, EFI_SYSTEM_TABLE* ST)
{
    //send systemTable to lib
    ImageHandle = IH;
    SystemTable = ST;
    InitializeGOP();
	
    ResetScreen();	

    /* delete the ugly cursor */
    Status = SystemTable->ConOut->EnableCursor(SystemTable->ConOut, 0);

	/* dark screen */

	fillrect(gop->Mode->FrameBufferBase, gop->Mode->Info->HorizontalResolution, gop->Mode->Info->VerticalResolution, 0, 0, 0, 0, 0);

    InitializeFileSystem();

    if(!initFont(L"zap-light16.psf")){
        ColdReboot();
    }
    readBMP(L"logo.bmp", 500, 0, 0, true);


	/* Load the kernel */

    EFI_FILE_PROTOCOL* efikernel = openFile(L"kernel.elf");
	
    size_t KernelSize;
    
    EFI_GUID gEfiFileInfoGuid = EFI_FILE_INFO_GUID;

    Elf64_Ehdr header;
	{
		UINTN FileInfoSize;
		EFI_FILE_INFO* FileInfo;
		Status = efikernel->GetInfo(efikernel, &gEfiFileInfoGuid, &FileInfoSize, NULL);
		Print(CheckStandardEFIError(Status));
		SystemTable->BootServices->AllocatePool(EfiLoaderData, FileInfoSize, (void**)&FileInfo);
		Status = efikernel->GetInfo(efikernel, &gEfiFileInfoGuid, &FileInfoSize, (void**)&FileInfo);
		Print(CheckStandardEFIError(Status));
		KernelSize = FileInfo->Size;
		UINTN size = sizeof(header);
		Status = efikernel->Read(efikernel, &size, &header);
		Print(CheckStandardEFIError(Status));
	}

    Elf64_Phdr* phdrs;
	{
		Status = efikernel->SetPosition(efikernel, header.e_phoff);
		Print(CheckStandardEFIError(Status));
		UINTN size = header.e_phnum * header.e_phentsize;
		SystemTable->BootServices->AllocatePool(EfiLoaderData, size, (void**)&phdrs);
		Status = efikernel->Read(efikernel, &size, phdrs);
		Print(CheckStandardEFIError(Status));
	}

    for (
		Elf64_Phdr* phdr = phdrs;
		(char*)phdr < (char*)phdrs + header.e_phnum * header.e_phentsize;
		phdr = (Elf64_Phdr*)((char*)phdr + header.e_phentsize)
	)
	{
		switch (phdr->p_type){
			case PT_LOAD:
			{
				int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
				Elf64_Addr segment = phdr->p_paddr;
				Status = SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);
				Print(CheckStandardEFIError(Status));
				Status = efikernel->SetPosition(efikernel, phdr->p_offset);
				Print(CheckStandardEFIError(Status));
				UINTN size = phdr->p_filesz;
				Status = efikernel->Read(efikernel, &size, (void*)segment);
				Print(CheckStandardEFIError(Status));	
				break;
			}
		}
	}
    
    closeFile(efikernel);

	EFI_MEMORY_DESCRIPTOR* Map = NULL;
	UINTN MapSize = 0;
	UINTN MapKey;
	UINTN DescriptorSize;
	UINT32 DescriptorVersion;
	{
		Status = SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
		Print(CheckStandardEFIError(Status));
		Status = SystemTable->BootServices->AllocatePool(EfiLoaderData, MapSize, (void**)&Map);
		Print(CheckStandardEFIError(Status));

		Status = EFI_LOAD_ERROR;
		while(Status != EFI_SUCCESS){
			Status = SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
			Print(CheckStandardEFIError(Status));
		}	
	}
	

    __attribute__((sysv_abi)) void (*KernelBinFile)(BootInfo*) = (( __attribute__((sysv_abi)) void (*)(BootInfo*)) header.e_entry);
	
	/* get rsdp */
	EFI_CONFIGURATION_TABLE* configTable = SystemTable->ConfigurationTable;
	void* rsdp = NULL; 
	EFI_GUID Acpi2TableGuid = ACPI_20_TABLE_GUID;

	for (UINTN index = 0; index < SystemTable->NumberOfTableEntries; index++){
		if (CompareGuid(&configTable[index].VendorGuid, &Acpi2TableGuid)){
			if (strcmp((CHAR8*)"RSD PTR ", (CHAR8*)configTable->VendorTable, 8)){
				rsdp = (void*)configTable->VendorTable;
			}
		}
		configTable++;
	}

	BootInfo bootInfo; 

	framebuffer->BaseAddress = (void*)gop->Mode->FrameBufferBase;
	framebuffer->FrameBufferSize = gop->Mode->FrameBufferSize;
    framebuffer->Width = gop->Mode->Info->HorizontalResolution;
    framebuffer->Height = gop->Mode->Info->VerticalResolution;
    framebuffer->PixelsPerScanLine = gop->Mode->Info->PixelsPerScanLine;

	bootInfo.KernelStart = header.e_entry;
	bootInfo.KernelSize = KernelSize;
	bootInfo.framebuffer = framebuffer;
	bootInfo.psf1_Font = LinuxFont;
	bootInfo.mMap = Map;
	bootInfo.mMapSize = MapSize;
	bootInfo.mMapDescSize = DescriptorSize;
    bootInfo.rsdp = rsdp;

	Status = SystemTable->BootServices->ExitBootServices(IH, MapKey);
	Print(CheckStandardEFIError(Status));

    KernelBinFile(&bootInfo);
    
    Shutdown();
	return 0;
}

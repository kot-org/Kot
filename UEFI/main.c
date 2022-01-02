#include "types.h"
#include "efi.h"
#include "bootinfo.h"
#include "errorCodes.h"
#include "lib.h"
#include "elf.h"
#include "memory.h"

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

	/* Load paging */
	EFI_MEMORY_DESCRIPTOR* Map = NULL;
	UINTN MapSize = 0;
	UINTN MapKey;
	UINTN DescriptorSize;
	UINT32 DescriptorVersion;
	Status = SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
	Print(CheckStandardEFIError(Status));
	Status = SystemTable->BootServices->AllocatePool(EfiLoaderData, MapSize, (void**)&Map);
	Print(CheckStandardEFIError(Status));

	Status = EFI_LOAD_ERROR;
	while(Status != EFI_SUCCESS){
		Status = SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
		Print(CheckStandardEFIError(Status));
	}	

	void* PML4 = InitMemory(Map, MapSize, DescriptorSize);

	uint64_t MapEntries = MapSize / DescriptorSize;
	GetMemorySize(Map, MapEntries, DescriptorSize);
	/* Load the kernel */

    EFI_FILE_PROTOCOL* efikernel = openFile(L"kernel.elf");
    
    EFI_GUID gEfiFileInfoGuid = EFI_FILE_INFO_GUID;

	uint64_t VirtualKernelStart = 0;
	uint64_t VirtualKernelEnd = 0;

    Elf64_Ehdr header;
	{
		UINTN FileInfoSize;
		EFI_FILE_INFO* FileInfo;
		Status = efikernel->GetInfo(efikernel, &gEfiFileInfoGuid, &FileInfoSize, NULL);
		Print(CheckStandardEFIError(Status));
		SystemTable->BootServices->AllocatePool(EfiLoaderData, FileInfoSize, (void**)&FileInfo);
		Status = efikernel->GetInfo(efikernel, &gEfiFileInfoGuid, &FileInfoSize, (void**)&FileInfo);
		Print(CheckStandardEFIError(Status));
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
				Elf64_Addr segment = phdr->p_paddr;
				if(VirtualKernelStart == NULL){
					VirtualKernelStart = segment;
				}
				Status = efikernel->SetPosition(efikernel, phdr->p_offset);

				int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
				void* PhysicalBuffer = allocatePages(pages);
				for(uint64_t i = 0; i < pages; i++){
					MapMemory(PML4, (void*)segment + i * 0x1000, (void*)PhysicalBuffer + i * 0x1000);
				}	
				VirtualKernelEnd = segment + pages * 0x1000;
				UINTN size = phdr->p_filesz;
				Status = efikernel->Read(efikernel, &size, (void*)PhysicalBuffer);
				Print(CheckStandardEFIError(Status));
			}
		}
	}

    
    __attribute__((sysv_abi)) void (*KernelBinFile)(struct BootInfo*) = (( __attribute__((sysv_abi)) void (*)(struct BootInfo*)) header.e_entry);
    closeFile(efikernel);
	
	/* get rsdp */
	EFI_CONFIGURATION_TABLE* configTable = SystemTable->ConfigurationTable;
	void* rsdp = NULL; 
	void* smbios = NULL; 
	EFI_GUID Acpi2TableGuid = ACPI_20_TABLE_GUID;
	EFI_GUID SMBIOSGUID = SMBIOS_TABLE_GUID;

	for (UINTN index = 0; index < SystemTable->NumberOfTableEntries; index++){
		if (CompareGuid(&configTable[index].VendorGuid, &Acpi2TableGuid)){
			if (strcmp((CHAR8*)"RSD PTR ", (CHAR8*)configTable->VendorTable, 8)){
				rsdp = (void*)configTable->VendorTable;
			}
		}
		if(CompareGuid(&configTable[index].VendorGuid, &SMBIOSGUID)){
			if(strcmp((CHAR8*)"_SM_", (CHAR8*)configTable->VendorTable,4)) {
				smbios = (void*)configTable->VendorTable;
			}
		}
		configTable++;
	}
	

	UINTN BootInfoSize = sizeof(struct BootInfo);
	struct BootInfo* bootInfo = NULL;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, BootInfoSize, (void**)&bootInfo); 


	Status = SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
	Print(CheckStandardEFIError(Status));
	Status = SystemTable->BootServices->AllocatePool(EfiLoaderData, MapSize, (void**)&Map);
	Print(CheckStandardEFIError(Status));

	Status = EFI_LOAD_ERROR;
	while(Status != EFI_SUCCESS){
		Status = SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
		Print(CheckStandardEFIError(Status));
	}	

	/* Boot info*/ 
		/* GOP */
		bootInfo->framebuffer.BaseAddress = (void*)gop->Mode->FrameBufferBase;
		bootInfo->framebuffer.FrameBufferSize = gop->Mode->FrameBufferSize;
		bootInfo->framebuffer.Height = gop->Mode->Info->VerticalResolution;
		bootInfo->framebuffer.Width = gop->Mode->Info->HorizontalResolution;
		bootInfo->framebuffer.PixelsPerScanLine = gop->Mode->Info->PixelsPerScanLine;

		/* Debug font */
		bootInfo->psf1_Font = LinuxFont;

		/* Mermory */
			/* Map */
			bootInfo->memoryInfo.mMap = Map;
			bootInfo->memoryInfo.mMapSize = MapSize;
			bootInfo->memoryInfo.mMapDescSize = DescriptorSize;

			bootInfo->memoryInfo.UEFI_CR3 = PML4;

			bootInfo->memoryInfo.VirtualKernelStart = VirtualKernelStart;
			bootInfo->memoryInfo.VirtualKernelEnd = VirtualKernelEnd;
		bootInfo->rsdp = rsdp;
		bootInfo->smbios = smbios;

	SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
	SystemTable->BootServices->ExitBootServices(ImageHandle, MapKey);

	KernelBinFile(bootInfo);
    
    Shutdown();
	return 0;
}
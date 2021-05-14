// UEFI From Scratch Series - ThatOSDev ( 2021 )
// https://github.com/ThatOSDev/C_UEFI

#include "efi.h"
#include "ErrorCodes.h"
#include "libs.h"
#include "elf.h"

UINTN strcmp(CHAR8* a, CHAR8* b, UINTN length){
	for (UINTN i = 0; i < length; i++){
		if (*a != *b) return 0;
	}
	return 1;
}

INT32 CompareGuid(EFI_GUID *Guid1, EFI_GUID *Guid2)
{
    INT32       *g1, *g2, r;

    //
    // Compare 32 bits at a time
    //

    g1 = (INT32 *) Guid1;
    g2 = (INT32 *) Guid2;

    r  = g1[0] - g2[0];
    r |= g1[1] - g2[1];
    r |= g1[2] - g2[2];
    r |= g1[3] - g2[3];

    return r;
}
    
EFI_STATUS efi_main(EFI_HANDLE IH, EFI_SYSTEM_TABLE *ST)
{
    ImageHandle = IH;
    SystemTable = ST;
    
    ResetScreen();
    
    SetColor(EFI_WHITE);
    SetTextPosition(3, 2);
    Print(L"Welcome to the UEFI From Scratch in C Series");

    InitializeFILESYSTEM();

    SetColor(EFI_GREEN);
    SetTextPosition(10, 22);
    Print(L"Hit Any Key to Initialize GOP, FONT and MEMAP");
    HitAnyKey();
    
    ResetScreen();
    
    InitializeGOP();
    
    TestFont();
    
    RunMemMap();
    
    // Kernel stuff here
    EFI_FILE_PROTOCOL* efikernel = openFile(L"kernel.elf");


    SetColor(EFI_GREEN);
    Print(L"\r\nLoading Poncho's ELF Kernel ... ");
    Print(CheckStandardEFIError(Status));
    
    
    EFI_GUID gEfiFileInfoGuid = EFI_FILE_INFO_ID;

    Elf64_Ehdr header;
	{
		UINTN FileInfoSize;
		EFI_FILE_INFO* FileInfo;
		efikernel->GetInfo(efikernel, &gEfiFileInfoGuid, &FileInfoSize, NULL);
		SystemTable->BootServices->AllocatePool(EfiLoaderData, FileInfoSize, (void**)&FileInfo);
		efikernel->GetInfo(efikernel, &gEfiFileInfoGuid, &FileInfoSize, (void**)&FileInfo);

		UINTN size = sizeof(header);
		efikernel->Read(efikernel, &size, &header);
	}

    Elf64_Phdr* phdrs;
	{
		efikernel->SetPosition(efikernel, header.e_phoff);
		UINTN size = header.e_phnum * header.e_phentsize;
		SystemTable->BootServices->AllocatePool(EfiLoaderData, size, (void**)&phdrs);
		efikernel->Read(efikernel, &size, phdrs);
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
				SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);

				efikernel->SetPosition(efikernel, phdr->p_offset);
				UINTN size = phdr->p_filesz;
				efikernel->Read(efikernel, &size, (void*)segment);
				break;
			}
		}
	}
    
    closeFile(efikernel);

    // Video 3
    // __attribute__((sysv_abi)) void (*KernelBinFile)(Framebuffer*, PSF1_FONT*) = (( __attribute__((sysv_abi)) void (*)(Framebuffer*, PSF1_FONT*) ) header.e_entry);
    
    // Video 18
    __attribute__((sysv_abi)) void (*KernelBinFile)(BootInfo*) = (( __attribute__((sysv_abi)) void (*)(BootInfo*) ) header.e_entry);

    SystemTable->BootServices->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);
    MemoryMapSize += 2 * DescriptorSize;
    SystemTable->BootServices->AllocatePool(EfiLoaderData, MemoryMapSize, (void**)&MemoryMap);
    SystemTable->BootServices->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);

	
	void* rsdp = NULL; 

    EFI_GUID Acpi2TableGuid = ACPI_20_TABLE_GUID;
    for(UINTN i = 0; i < SystemTable->NumberOfTableEntries; i++) {
        EFI_CONFIGURATION_TABLE t = SystemTable->ConfigurationTable[i];
        if(CompareGuid(&Acpi2TableGuid, &t.VendorGuid) == 0) {
            rsdp = t.VendorTable;
        }
    }


	bootInfo.framebuffer = &newBuffer;
	bootInfo.psf1_Font = LinuxFont;
	bootInfo.mMap = MemoryMap;
	bootInfo.mMapSize = MemoryMapSize;
	bootInfo.mMapDescSize = DescriptorSize;
    bootInfo.rsdp = rsdp;
    
	SystemTable->BootServices->ExitBootServices(ImageHandle, MapKey);

    // Video 3
    // KernelBinFile(&newBuffer, LinuxFont);
    
    // Video 18
    KernelBinFile(&bootInfo);


    SetColor(EFI_GREEN);
    SetTextPosition(10, 24);
    Print(L"Hit Any Key to exit");
    HitAnyKey();

    COLD_REBOOT();
   
	return 0;
}

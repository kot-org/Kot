#include <multiboot/core/multiboot.h>

struct ukl_boot_structure_t BootData;

extern "C" void multiboot_entry(uint32_t Magic, uint32_t BootDataBase){
    uint64_t KernelEntryPoint = NULL;
    uint64_t Stack = NULL;
    uint64_t KernelBuffer = NULL;
    for(struct multiboot_tag* tag = (struct multiboot_tag*)(BootDataBase + 8); tag->type != MULTIBOOT_TAG_TYPE_END; tag = (struct multiboot_tag*) ((multiboot_uint8_t*) tag + ((tag->size + 7) & ~7))){
        switch (tag->type){
            case MULTIBOOT_TAG_TYPE_MODULE:{
                struct multiboot_tag_module* module_header = (struct multiboot_tag_module*)tag; 
                uint8_t* module_buffer = (uint8_t*)module_header->mod_start;
                bool IsElf = (module_buffer[0] == EI_MAG0 && module_buffer[1] == EI_MAG1 && module_buffer[2] == EI_MAG2 && module_buffer[3] == EI_MAG3);
                if(IsElf){
                    KernelBuffer = module_header->mod_start;
                }else{
                    BootData.initrd.base = module_header->mod_start;
                    BootData.initrd.size = module_header->mod_end - module_header->mod_start;
                }
                break;
            }
            case MULTIBOOT_TAG_TYPE_MMAP:
                InitializeMemory(BootDataBase, ((struct multiboot_tag_mmap*)tag), &Stack);
                break;
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
                BootData.framebuffer.framebuffer_base = (uint64_t)((struct multiboot_tag_framebuffer_common*)tag)->framebuffer_addr;
                BootData.framebuffer.framebuffer_Width = (uint64_t)((struct multiboot_tag_framebuffer_common*)tag)->framebuffer_Width;
                BootData.framebuffer.framebuffer_Height = (uint64_t)((struct multiboot_tag_framebuffer_common*)tag)->framebuffer_Height;
                BootData.framebuffer.framebuffer_Pitch = (uint64_t)((struct multiboot_tag_framebuffer_common*)tag)->framebuffer_Pitch;
                BootData.framebuffer.framebuffer_Bpp = (uint64_t)((struct multiboot_tag_framebuffer_common*)tag)->framebuffer_Bpp;
                break;
            case MULTIBOOT_TAG_TYPE_SMBIOS:
                BootData.SMBIOS.base = (uint64_t)&((struct multiboot_tag_smbios*)tag)->tables[0];
                BootData.SMBIOS.size = (uint64_t)((struct multiboot_tag_smbios*)tag)->size - (sizeof(struct multiboot_tag_smbios) - 1);
                break;
            case MULTIBOOT_TAG_TYPE_ACPI_OLD:
                BootData.RSDP.type = UKL_OLD_ACPI;
                BootData.RSDP.base = (uint64_t)&((struct multiboot_tag_old_acpi*)tag)->rsdp[0];
                BootData.RSDP.size = (uint64_t)((struct multiboot_tag_old_acpi*)tag)->size - (sizeof(struct multiboot_tag_old_acpi) - 1);
                break;
            case MULTIBOOT_TAG_TYPE_ACPI_NEW:
                BootData.RSDP.type = UKL_NEW_ACPI;
                BootData.RSDP.base = (uint64_t)&((struct multiboot_tag_new_acpi*)tag)->rsdp[0];
                BootData.RSDP.size = (uint64_t)((struct multiboot_tag_new_acpi*)tag)->size - (sizeof(struct multiboot_tag_new_acpi) - 1);
                break;
            default:
                break;
        }
    }


    loadElf((uintptr_t)KernelBuffer, &KernelEntryPoint, &BootData.kernel_address);

    Pmm_ExtractsInfo(&BootData);
    Vmm_ExtractsInfo(&BootData);

    boot_kernel((uint32_t)vmm_PageTable, KernelEntryPoint, Stack, (uint64_t)&BootData);
}
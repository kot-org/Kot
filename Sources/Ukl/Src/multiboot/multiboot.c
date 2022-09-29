#include <multiboot/multiboot.h>


void multiboot_entry(uint32_t Magic, uint32_t BootDataBase){
    struct ukl_boot_structure_t BootData;
    for(struct multiboot_tag* tag = (struct multiboot_tag*)(BootDataBase + 8); tag->type != MULTIBOOT_TAG_TYPE_END; tag = (struct multiboot_tag*) ((multiboot_uint8_t*) tag + ((tag->size + 7) & ~7))){
        switch (tag->type){
            case MULTIBOOT_TAG_TYPE_MODULE:
                
                break;
            case MULTIBOOT_TAG_TYPE_MMAP:
                
                break;
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
                BootData.Framebuffer.framebuffer_base = ((struct multiboot_tag_framebuffer_common*)tag)->framebuffer_addr;
                BootData.Framebuffer.framebuffer_width = ((struct multiboot_tag_framebuffer_common*)tag)->framebuffer_width;
                BootData.Framebuffer.framebuffer_height = ((struct multiboot_tag_framebuffer_common*)tag)->framebuffer_height;
                BootData.Framebuffer.framebuffer_pitch = ((struct multiboot_tag_framebuffer_common*)tag)->framebuffer_pitch;
                BootData.Framebuffer.framebuffer_bpp = ((struct multiboot_tag_framebuffer_common*)tag)->framebuffer_bpp;
                break;
            case MULTIBOOT_TAG_TYPE_SMBIOS:
                BootData.SMBIOS.base = &((struct multiboot_tag_smbios*)tag)->tables[0];
                BootData.SMBIOS.size = ((struct multiboot_tag_smbios*)tag)->size - (sizeof(struct multiboot_tag_smbios) - 1);
                break;
            case MULTIBOOT_TAG_TYPE_ACPI_OLD:
                BootData.RSDP.type = UKL_OLD_ACPI;
                BootData.RSDP.base = &((struct multiboot_tag_old_acpi*)tag)->rsdp[0];
                BootData.RSDP.size = ((struct multiboot_tag_old_acpi*)tag)->size - (sizeof(struct multiboot_tag_old_acpi) - 1);
                break;
            case MULTIBOOT_TAG_TYPE_ACPI_NEW:
                BootData.RSDP.type = UKL_NEW_ACPI;
                BootData.RSDP.base = &((struct multiboot_tag_new_acpi*)tag)->rsdp[0];
                BootData.RSDP.size = ((struct multiboot_tag_new_acpi*)tag)->size - (sizeof(struct multiboot_tag_new_acpi) - 1);
                break;
            default:
                break;
        }
    }
}
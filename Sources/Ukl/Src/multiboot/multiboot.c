#include <multiboot/multiboot.h>

#define COM1 0x3f8

static inline void IoWrite8(uint16_t port, uint8_t data){
    __asm__ volatile("outb %b0, %w1" : : "a" (data), "Nd" (port));
}

void Initialize() {
    IoWrite8(COM1 + 1, 0x00);    // Disable all interrupts
    IoWrite8(COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    IoWrite8(COM1 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    IoWrite8(COM1 + 1, 0x00);    //                  (hi byte)
    IoWrite8(COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
    IoWrite8(COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    IoWrite8(COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
    IoWrite8(COM1 + 4, 0x0F);    // Set in normal mode
}

void Write(char chr) {
    IoWrite8(COM1, chr);
}

void Print(const char* chr) {
    for(uint64_t i = 0; chr[i] != '\0'; i++){
        Write(chr[i]);
        if(chr[i] == '\n') Write('\r');   
    }
}

void multiboot_entry(uint32_t Magic, uint32_t BootDataBase){
    struct ukl_boot_structure_t BootData;
    for(struct multiboot_tag* tag = (struct multiboot_tag*)(BootDataBase + 8); tag->type != MULTIBOOT_TAG_TYPE_END; tag = (struct multiboot_tag*) ((multiboot_uint8_t*) tag + ((tag->size + 7) & ~7))){
        switch (tag->type){
            case MULTIBOOT_TAG_TYPE_MODULE:
                Print("5\n");
                
                break;
            case MULTIBOOT_TAG_TYPE_MMAP:
                Print("4\n");
                
                break;
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
                Print("0\n");
                BootData.Framebuffer.framebuffer_base = ((struct multiboot_tag_framebuffer_common*)tag)->framebuffer_addr;
                BootData.Framebuffer.framebuffer_width = ((struct multiboot_tag_framebuffer_common*)tag)->framebuffer_width;
                BootData.Framebuffer.framebuffer_height = ((struct multiboot_tag_framebuffer_common*)tag)->framebuffer_height;
                BootData.Framebuffer.framebuffer_pitch = ((struct multiboot_tag_framebuffer_common*)tag)->framebuffer_pitch;
                BootData.Framebuffer.framebuffer_bpp = ((struct multiboot_tag_framebuffer_common*)tag)->framebuffer_bpp;
                break;
            case MULTIBOOT_TAG_TYPE_SMBIOS:
                Print("1\n");
                BootData.SMBIOS.base = &((struct multiboot_tag_smbios*)tag)->tables[0];
                BootData.SMBIOS.size = ((struct multiboot_tag_smbios*)tag)->size - (sizeof(struct multiboot_tag_smbios) - 1);
                break;
            case MULTIBOOT_TAG_TYPE_ACPI_OLD:
                Print("2\n");
                BootData.RSDP.type = UKL_OLD_ACPI;
                BootData.RSDP.base = &((struct multiboot_tag_old_acpi*)tag)->rsdp[0];
                BootData.RSDP.size = ((struct multiboot_tag_old_acpi*)tag)->size - (sizeof(struct multiboot_tag_old_acpi) - 1);
                break;
            case MULTIBOOT_TAG_TYPE_ACPI_NEW:
                Print("3\n");
                BootData.RSDP.type = UKL_NEW_ACPI;
                BootData.RSDP.base = &((struct multiboot_tag_new_acpi*)tag)->rsdp[0];
                BootData.RSDP.size = ((struct multiboot_tag_new_acpi*)tag)->size - (sizeof(struct multiboot_tag_new_acpi) - 1);
                break;
            default:
                break;
        }
    }
}
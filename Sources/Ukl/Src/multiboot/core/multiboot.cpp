#include <multiboot/core/multiboot.h>

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

char* itoa(int32_t n, char* buffer, int basenumber){
	int32_t hold;
	int32_t i, j;
	hold = n;
	i = 0;

    bool IsNegative = (n < 0);

    if(IsNegative){
        n = -n;
    }
	
	do{
		hold = n % basenumber;
		buffer[i++] = (hold < 10) ? (hold + '0') : (hold + 'a' - 10);
	} while(n /= basenumber);

    if(IsNegative){
        buffer[i++] = '-';
    }

	buffer[i--] = NULL;
	
	for(j = 0; j < i; j++, i--)
	{
		hold = buffer[j];
		buffer[j] = buffer[i];
		buffer[i] = hold;
	}


    return buffer;
}

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
                BootData.framebuffer.framebuffer_width = (uint64_t)((struct multiboot_tag_framebuffer_common*)tag)->framebuffer_width;
                BootData.framebuffer.framebuffer_height = (uint64_t)((struct multiboot_tag_framebuffer_common*)tag)->framebuffer_height;
                BootData.framebuffer.framebuffer_pitch = (uint64_t)((struct multiboot_tag_framebuffer_common*)tag)->framebuffer_pitch;
                BootData.framebuffer.framebuffer_bpp = (uint64_t)((struct multiboot_tag_framebuffer_common*)tag)->framebuffer_bpp;
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
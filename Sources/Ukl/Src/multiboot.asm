[BITS 32]

%include "Src/multiboot2.inc"

GLOBAL _start
EXTERN multiboot_kernel_entry

SECTION .multiboot

multiboot_header:
    ; required header start
    ALIGN MULTIBOOT_HEADER_ALIGN
	uint32   MULTIBOOT2_HEADER_MAGIC                    ; magic
    uint32   MULTIBOOT_ARCHITECTURE_I386                ; architecture
    uint32   multiboot_header_end - multiboot_header    ; header_length
    uint32   -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT_ARCHITECTURE_I386 + (multiboot_header_end - multiboot_header)) ; checksum
    
    ; optional multiboot tags
    align_module_tag_start:  
        ALIGN MULTIBOOT_HEADER_ALIGN
        uint16 MULTIBOOT_HEADER_TAG_MODULE_ALIGN
        uint16 MULTIBOOT_HEADER_TAG_OPTIONAL
        uint32 align_module_tag_end - align_module_tag_start
    align_module_tag_end:

    framebuffer_tag_start:  
        ALIGN MULTIBOOT_HEADER_ALIGN
        uint16 MULTIBOOT_HEADER_TAG_FRAMEBUFFER
        uint16 MULTIBOOT_HEADER_TAG_OPTIONAL
        uint32 framebuffer_tag_end - framebuffer_tag_start
        uint32 0
        uint32 0
        uint32 32
    framebuffer_tag_end:
    
    ; required header end
    ALIGN MULTIBOOT_HEADER_ALIGN
    uint16 MULTIBOOT_HEADER_TAG_END     ; type
    uint16 0                            ; flags
    uint32 8                            ; sizeof(uint32_t)
multiboot_header_end:

SECTION .text

_start:
    call multiboot_kernel_entry
	hlt

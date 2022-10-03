#pragma once
#include <kot/types.h>

#define UKL_MODULE_STRING_SIZE 128

#define UKL_MMAP_AVAILABLE              1
#define UKL_MMAP_USED                   2
#define UKL_MMAP_RESERVED               3

#define UKL_OLD_ACPI 0
#define UKL_NEW_ACPI 1

/* Structures */

struct ukl_initrd_t{
	uint64_t base;
	size64_t size;
}__attribute__((packed));


struct ukl_kernel_address_t{
    uint64_t virtual_base_address;
    uint64_t virtual_end_address;
}__attribute__((packed));


struct ukl_framebuffer_t{
    uint64_t framebuffer_base;
    uint64_t framebuffer_width;
    uint64_t framebuffer_height;
    uint64_t framebuffer_pitch;
    uint64_t framebuffer_bpp;
}__attribute__((packed));


struct ukl_mmap_info_t{
    uint64_t base;
    uint64_t type;
    size64_t length;
    uint64_t map_next_entry;
}__attribute__((packed));


struct ukl_memmory_info_t{
    uint64_t bitmap_address;
    size64_t bitmap_size;
    uint64_t HHDM;
    uint64_t page_table;
    uint64_t page_count_total;
    uint64_t map_entries_count;
    uint64_t map_main_entry;
}__attribute__((packed));


struct ukl_rsdp_t{
    uint64_t type;
    uint64_t base;
    size64_t size;
}__attribute__((packed));


struct ukl_smbios_t{
    uint64_t base;
    size64_t size;
}__attribute__((packed));



/* Boot structure */
struct ukl_boot_structure_t{
	struct ukl_kernel_address_t kernel_address;
    struct ukl_framebuffer_t framebuffer;
    struct ukl_memmory_info_t memory_info;
    uint64_t bootloader_signature;
    struct ukl_rsdp_t RSDP;
    struct ukl_smbios_t SMBIOS;
    struct ukl_initrd_t initrd;
}__attribute__((packed));
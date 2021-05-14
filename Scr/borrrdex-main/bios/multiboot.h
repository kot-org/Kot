#pragma once

#include <cstdint>

typedef struct {
    uint32_t flags;
    uint32_t memory_low;
    uint32_t memory_high;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t module_count;
    uint32_t module_addr;
    union
    {
        struct
        {
            uint32_t tab_size;
            uint32_t str_size;
            uint32_t addr;
            uint32_t pad;
        } a;
        struct
        {
            uint32_t num;
            uint32_t size;
            uint32_t addr;
            uint32_t shndx;
        } e;
    } symbols;

    uint32_t memory_map_length;
    uint32_t memory_map_addr;

    uint32_t drives_length;
    uint32_t drives_addr;

    uint32_t config_table;

    uint32_t bootloader_name;

    uint32_t apm_table;

    uint32_t vbe_controller_info;
    uint32_t vbe_mode_info;
    uint32_t vbe_mode;
    uint32_t vbe_interface_segment;
    uint32_t vbe_interface_offset;
    uint32_t vbe_interface_length;
} __attribute__((packed)) multiboot_info_t;

namespace multiboot {
    constexpr uint16_t INFO_MEMORY          = 0x1;
    constexpr uint16_t INFO_BOOTDEVICE      = 0x2;
    constexpr uint16_t INFO_CMDLINE         = 0x3;
    constexpr uint16_t INFO_MODULES         = 0x4;

    constexpr uint16_t INFO_AOUT            = 0x10;
    constexpr uint16_t INFO_ELF             = 0x20;

    constexpr uint16_t INFO_MEM_MAP         = 0x40;
    constexpr uint16_t INFO_DRIVE_INFO      = 0x80;
    constexpr uint16_t INFO_CONFIG_TABLE    = 0x100;
    constexpr uint16_t INFO_BOOT_LDR_NAME   = 0x200;
    constexpr uint16_t INFO_APM_TABLE       = 0x400;
    constexpr uint16_t INFO_VIDEO_INFO      = 0x800;

    constexpr uint32_t MAGIC                = 0x2badb002;
}
#pragma once

#ifndef __cplusplus
#error C++ only
#endif

#include <cstdint>

constexpr uint8_t GDT_MAX_DESCRIPTORS   = 16;       ///< The number of entries that can fit into the GDT

/**
 * Accessed bit. Just set to 0. The CPU sets this to 1 when the segment is accessed. 
 */
constexpr uint8_t GDT_DESC_ACCESS       = 0x01;

/**
 * Code Segments - 1 for read access, 0 for no access
 * Data Segments - 1 for write access, 0 for no access
 */
constexpr uint8_t GDT_DESC_READWRITE    = 0x02;

/**
 * Code Segments [Conformance]:
 *      0 means this segment can only be jumped into from the same privilege level
 *      1 means *lower* privilege levels are also ok (i.e. 3 -> 2 is ok, but 0 -> 2 is not)
 * 
 * Data Segments [Direction]:
 *      0 means The segment grows upwards, 1 means it grows downwards
 */
constexpr uint8_t GDT_DESC_DC           = 0x04;

/**
 * If 1, this is a code segment, 0 means data segment
 */
constexpr uint8_t GDT_DESC_EXECUTABLE   = 0x08;

/**
 * If 1, this is either a code or data segment, 0 means system segment
 */
constexpr uint8_t GDT_DESC_CODEDATA     = 0x10;

/**
 * If this value is set, the segment is ring level 3 (0 if not set)
 */
constexpr uint8_t GDT_DESC_DPL          = 0x60;

/**
 * This value must be 1 on all valid selectors
 */
constexpr uint8_t GDT_DESC_PRESENT      = 0x80;

/**
 * When this mask is applied, the hi portion of limit is filtered out,
 * and only the leftover flags remains set
 */
constexpr uint8_t GDT_GRAN_LIMITHIMAST  = 0x0F;

/**
 * This bit is OS specific, and has no inherent meaning
 */
constexpr uint8_t GDT_GRAN_OS           = 0x10;

/**
 * If set, this is a 64-bit segment.  Otherwise it could be 32-bit
 * if the 32-bit bit is set, or 16-bit if neither are set
 */
constexpr uint8_t GDT_GRAN_64BIT        = 0x20;

/**
 * If set, this is a 32-bit segment.  Otherwise it could be 64-bit
 * if the 64-bit bit is set, or 16-bit if neither are set
 */
constexpr uint8_t GDT_GRAN_32BIT        = 0x40;

/**
 * If set, the granularity of the limit is measured in [4 KiB] pages, 
 * otherwise it is measured in bytes
 */
constexpr uint8_t GDT_GRAN_4K           = 0x80;

// Predefined selectors
constexpr uint8_t GDT_SELECTOR_KERNEL_CODE  = (0x01 << 3);
constexpr uint8_t GDT_SELECTOR_KERNEL_DATA  = (0x02 << 3);
constexpr uint8_t GDT_SELECTOR_USER_CODE    = (0x05 << 3);
constexpr uint8_t GDT_SELECTOR_USER_DATA    = (0x04 << 3);


typedef struct gdt_descriptor {
    uint16_t limit;         ///< [Unused for 64-bit] The descriptor size (bits 0 - 15)
    uint16_t base_low;      ///< [Unused for 64-bit] The descriptor memory address (bits 0 -15)
    uint8_t base_mid;       ///< [Unused for 64-bit] The descriptor memory address (bits 16 - 23)
    uint8_t flags;          ///< The access flags (see GDT_DESC_*)
    uint8_t granularity;    ///< The remaining flags (see GDT_GRAN_*), and [unused for 64-bit] The descriptor size (bits 16 - 19)
    uint8_t base_high;      ///< [Unused for 64-bit] The descriptor memory address (bits 24 - 31)
} __attribute__((packed)) gdt_desc_t;

typedef struct gdt_system_descriptor {
    uint16_t limit_0;
    uint16_t addr_0;
    uint8_t addr_1;
    uint8_t type_0;
    uint8_t limit_1;
    uint8_t addr_2;
    uint32_t addr_3;
    uint32_t reserved;
} __attribute__((packed)) gdt_system_desc_t;

// Basically, this structure is an array of the above,
// defined as a block of memory
typedef struct gdt {
    uint16_t limit;         ///< The size in memory of all the gdt_desc_t entries (laid out sequentially)
    uint64_t base;          ///< The memory address of the first gdt_desc_t entry 
} __attribute__((packed)) gdt_t;

/**
 * Initializes the GDT.  Don't call more than once.  As a side effect,
 * the code segment is set to GDT_SELECTOR_KERNEL_CODE
 */
void gdt_init();

/**
 * Writes a new entry into the gdt_t structure.
 * @param base          [Unused for 64-bit] The descriptor memory address
 * @param limit         [Unused for 64-bit] The descriptor size
 * @param access        The access flags (see GDT_DESC_*)
 * @param granularity   The remaining flags (see GDT_GRAN_*)
 */
void gdt_install_descriptor(uint64_t base, uint64_t limit, uint8_t access, uint8_t granularity);

void gdt_install_tss(uint64_t base, uint64_t limit);

extern "C" const void* gdt_address();

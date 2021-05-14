#include "gdt.h"

typedef struct {
    uint16_t limit;
    uint32_t base;
} gdt_32_t;

static __attribute__((aligned(0x1000))) gdt_desc_t s_gdt_descriptors[GDT_MAX_DESCRIPTORS];
static gdt_t s_gdt;
static gdt_32_t s_gdt_32;
static uint32_t s_gdt_index;

constexpr uint8_t BASE_DESC = GDT_DESC_PRESENT | GDT_DESC_READWRITE | GDT_DESC_CODEDATA;
constexpr uint8_t BASE_GRAN = GDT_GRAN_64BIT | GDT_GRAN_4K;

extern "C" void __load_gdt(gdt_t* gdt);

extern "C" const void* gdt_address() {
    return &s_gdt_32;
}

void gdt_init() {
    s_gdt.limit = (sizeof(gdt_desc_t) * GDT_MAX_DESCRIPTORS) - 1;
    s_gdt.base = (uint64_t)&s_gdt_descriptors[0];

    // NULL first
    gdt_install_descriptor(0, 0, 0, 0);

    // KERNEL CODE
    gdt_install_descriptor(0, 0, BASE_DESC | GDT_DESC_EXECUTABLE, BASE_GRAN);

    // KERNEL DATA
    gdt_install_descriptor(0, 0, BASE_DESC, BASE_GRAN);

    // USER BASE
    gdt_install_descriptor(0, 0, 0, 0);

    // USER DATA
    gdt_install_descriptor(0, 0, BASE_DESC | GDT_DESC_DPL, BASE_GRAN);

    // USER CODE
    gdt_install_descriptor(0, 0, BASE_DESC | GDT_DESC_EXECUTABLE | GDT_DESC_DPL, BASE_GRAN);

    __load_gdt(&s_gdt);
}

void gdt_install_descriptor(uint64_t base, uint64_t limit, uint8_t access, uint8_t granularity) {
    if(s_gdt_index >= GDT_MAX_DESCRIPTORS) {
        return;
    }

    s_gdt_descriptors[s_gdt_index].base_low = (uint16_t)(base & 0xFFFF);
    s_gdt_descriptors[s_gdt_index].base_mid = (uint8_t)((base >> 16) & 0xFF);
    s_gdt_descriptors[s_gdt_index].base_high = (uint8_t)((base >> 24) & 0xFF);
    s_gdt_descriptors[s_gdt_index].limit = (uint16_t)(limit & 0xFFFF);

    s_gdt_descriptors[s_gdt_index].flags = access;
    s_gdt_descriptors[s_gdt_index].granularity = (uint8_t)((limit >> 16) & 0x0F);
    s_gdt_descriptors[s_gdt_index].granularity |= granularity & 0xF0;

    s_gdt_index++;
}

void gdt_install_tss(uint64_t base, uint64_t limit) {
    uint16_t tss_type = GDT_DESC_ACCESS | GDT_DESC_EXECUTABLE | GDT_DESC_PRESENT;
    gdt_system_desc_t* gdt_desc = (gdt_system_desc_t *)&s_gdt_descriptors[s_gdt_index];

    if(s_gdt_index >= GDT_MAX_DESCRIPTORS) {
        return;
    }

    gdt_desc->type_0 = (uint16_t)(tss_type & 0x00FF);

    gdt_desc->addr_0 = base & 0xFFFF;
    gdt_desc->addr_1 = (base & 0xFF0000) >> 16;
    gdt_desc->addr_2 = (base & 0xFF000000) >> 24;
    gdt_desc->addr_3 = base >> 32;

    gdt_desc->limit_0 = limit & 0xFFFF;
    gdt_desc->limit_1 = (limit & 0xF0000) >> 16;

    gdt_desc->reserved = 0;

    s_gdt_index += 2;
}
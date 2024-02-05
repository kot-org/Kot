#include <arch/include.h>
#include ARCH_INCLUDE(gdt.h)

#include <lib/lock.h>

static tss_t _tss = {
    .reserved = 0,
    .rsp = {},
    .reserved0 = 0,
    .ist = {},
    .reserved1 = 0,
    .reserved2 = 0,
    .reserved3 = 0,
    .iopb_offset = 0,
};

static __attribute__((aligned(4096))) gdt_t _gdt = {};

static gdtr_t _gdtr = {
    .limit = sizeof(gdt_t) - 1,
    .base = (uint64_t) &_gdt,
};

gdt_entry_t gdt_entry(uint32_t base, uint32_t limit, uint8_t granularity, uint8_t flags) {
    return (gdt_entry_t) {
        .limit_low = (uint16_t)((limit)&0xffff),
        .base_low = (uint16_t)((base)&0xffff),
        .base_middle = (uint8_t)(((base) >> 16) & 0xff),
        .flags = (flags),
        .limit_middle = ((limit) >> 16) & 0x0f,
        .granularity = (granularity),
        .base_high = (uint8_t)(((base) >> 24) & 0xff),
    };
}

gdt_entry_t gdt_null_entry(void) {
    return gdt_entry(0, 0, 0, 0);
}

gdt_entry_t gdt_simple_entry(uint8_t flags, uint8_t granularity) {
    return gdt_entry(0, 0, granularity, flags);
}

tss_entry_t gdt_tss_entry(uintptr_t tss_address) {
    return (tss_entry_t) {
        .length = sizeof(tss_t),
        .base_low = tss_address & 0xffff,
        .base_middle = (tss_address >> 16) & 0xff,
        .flags1 = 0b10001001,
        .flags2 = 0,
        .base_high = (tss_address >> 24) & 0xff,
        .base_upper = tss_address >> 32,
        .reserved = 0,
    };
}

void gdt_init_cpu(void) {
    gdt_update(&_gdtr);
}

void gdt_init(void) {
    _gdt.entries[GDT_NULL_0] = gdt_null_entry();
    _gdt.entries[GDT_KERNEL_CODE] = gdt_simple_entry(GDT_PRESENT | GDT_SEGMENT | GDT_READWRITE | GDT_EXECUTABLE, GDT_LONG_MODE_GRANULARITY);
    _gdt.entries[GDT_KERNEL_DATA] = gdt_simple_entry(GDT_PRESENT | GDT_SEGMENT | GDT_READWRITE, 0);

    _gdt.entries[GDT_NULL_1] = gdt_null_entry();
    _gdt.entries[GDT_USER_CODE] = gdt_simple_entry(GDT_PRESENT | GDT_SEGMENT | GDT_READWRITE | GDT_EXECUTABLE | GDT_USER, GDT_LONG_MODE_GRANULARITY);
    _gdt.entries[GDT_USER_DATA] = gdt_simple_entry(GDT_PRESENT | GDT_SEGMENT | GDT_READWRITE | GDT_USER, 0);
    
    _gdt.tss = gdt_tss_entry((uintptr_t)&_tss);

    gdt_init_cpu();
}

static spinlock_t _gdt_lock = SPINLOCK_INIT;

void gdt_load_tss(tss_t *tss) {
    spinlock_acquire(&_gdt_lock);
    _gdt.tss = gdt_tss_entry((uintptr_t) tss);
    tss_update();
    spinlock_release(&_gdt_lock);
}

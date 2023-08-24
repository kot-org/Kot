#ifndef _AMD64_GDT_H
#define _AMD64_GDT_H 1

#include <stdint.h>

#define GDT_ENTRY_COUNT 6
#define GDT_SEGMENT (0b00010000)
#define GDT_PRESENT (0b10000000)
#define GDT_USER (0b01100000)
#define GDT_EXECUTABLE (0b00001000)
#define GDT_READWRITE (0b00000010)
#define GDT_LONG_MODE_GRANULARITY 0b0010
#define GDT_FLAGS 0b1100

#define GDT_NULL_0 (0)
#define GDT_KERNEL_CODE (1)
#define GDT_KERNEL_DATA (2)
#define GDT_NULL_1 (3)
#define GDT_USER_DATA (4)
#define GDT_USER_CODE (5)

#define GDT_RING_3 (3)

typedef struct {
    uint32_t reserved;
    uint64_t rsp[3];
    uint64_t reserved0;
    uint64_t ist[7];
    uint32_t reserved1;
    uint32_t reserved2;
    uint16_t reserved3;
    uint16_t iopb_offset;
} __attribute__((packed)) tss_t;

typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) gdtr_t;

typedef struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t flags;
    uint8_t limit_middle : 4;
    uint8_t granularity : 4;
    uint8_t base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct {
    uint16_t length;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t flags1;
    uint8_t flags2;
    uint8_t base_high;
    uint32_t base_upper;
    uint32_t reserved;
} __attribute__((packed)) tss_entry_t;

typedef struct {
    gdt_entry_t entries[GDT_ENTRY_COUNT];
    tss_entry_t tss;
} __attribute__((packed)) gdt_t;

gdt_entry_t gdt_entry(uint32_t base, uint32_t limit, uint8_t granularity, uint8_t flags);
gdt_entry_t gdt_simple_entry(uint8_t flags, uint8_t granularity);
gdt_entry_t gdt_null_entry(void);
tss_entry_t gdt_tss_entry(uintptr_t tss_address);

void gdt_init_cpu(void);
void gdt_init(void);
void gdt_load_tss(tss_t *tss);

// implemented in "gdt.s"
void gdt_update(gdtr_t* gdtr);
void tss_update(void);

#endif // _AMD64_GDT_H

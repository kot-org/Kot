#ifndef _AMD64_ACPI_H
#define _AMD64_ACPI_H

struct acpi_rsdp2{
    uint8_t signature[8];
    uint8_t checksum;
    uint8_t oemid[6];
    uint8_t revision;
    uint32_t rsdt_address;
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    uint8_t reserved[3];
}__attribute__((packed));

struct acpi_sdt_header{
    uint8_t signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    uint8_t oemid[6];
    uint8_t oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
}__attribute__((packed));

struct acpi_rsdt{
    struct acpi_sdt_header header;
    uint32_t sdt_pointer[];
}__attribute__((packed));

struct acpi_xsdt{
    struct acpi_sdt_header header;
    uint64_t sdt_pointer[];
}__attribute__((packed));

struct acpi_generic_address_structure{
    uint8_t address_space;       // 0 - system memory, 1 - system i/o
    uint8_t bit_width;
    uint8_t bit_offset;
    uint8_t access_size;
    uint64_t address;
}__attribute__((packed));

struct acpi_madt_header{
    struct acpi_sdt_header header;
    uint32_t apic_address;
    uint32_t flags;
}__attribute__((packed));

struct acpi_hpet_header{
    struct acpi_sdt_header header;
    uint8_t revision_id;
    uint8_t comparator_count:5;
    uint8_t counter_size:1;
    uint8_t reserved:1;
    uint8_t legacy_replacement:1;
    uint16_t vendor_id;     
        
    struct acpi_generic_address_structure address;

    uint8_t hpet_number;
    uint16_t minimum_tick;
    uint8_t page_protection;
}__attribute__((packed));

extern struct acpi_rsdp2* acpi_rsdp;

void* acpi_find_table(struct acpi_rsdp2* rsdp, char* signature);

#endif
#include <stdint.h>
#include <stdbool.h>
#include <impl/vmm.h>
#include <lib/string.h>

#include <arch/include.h>
#include ARCH_INCLUDE(acpi.h)

struct acpi_rsdp2* acpi_rsdp = NULL;

void* acpi_find_table(struct acpi_rsdp2* rsdp, char* signature){
    struct acpi_sdt_header* sdt = NULL;
    bool is_xsdt = (rsdp->revision >= 1);
    uint64_t entries = 0;
    if(is_xsdt){
        sdt = (struct acpi_sdt_header*)vmm_get_virtual_address((void*)(uint64_t)rsdp->xsdt_address);
        entries = (sdt->length - sizeof(struct acpi_sdt_header)) / sizeof(uint64_t);
    }else{
        sdt = (struct acpi_sdt_header*)vmm_get_virtual_address((void*)(uint64_t)rsdp->rsdt_address);
        entries = (sdt->length - sizeof(struct acpi_sdt_header)) / sizeof(uint32_t);
    }        

    struct acpi_sdt_header* header = NULL;

    for(uint64_t i = 0; i < entries; i++){
        if(is_xsdt){
            header = (struct acpi_sdt_header*)((uint64_t)((struct acpi_xsdt*)sdt)->sdt_pointer[i]);
        }else{
            header = (struct acpi_sdt_header*)((uint64_t)((uint32_t)((struct acpi_rsdt*)sdt)->sdt_pointer[i]));
        }
        header = (struct acpi_sdt_header*)vmm_get_virtual_address((void*)header);

        if(!strncmp((char*)header->signature, signature, 4)){
            return header;
        }
    }
    return 0;
} 
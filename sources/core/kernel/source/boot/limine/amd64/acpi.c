#include <boot/limine.h>

#include <arch/include.h>
#include ARCH_INCLUDE(acpi.h)

static volatile struct limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0
};

void acpi_init(void){
    acpi_rsdp = (struct acpi_rsdp2*)rsdp_request.response->address;
}
#include <boot/limine.h>

#include <impl/vmm.h>
#include <global/pmm.h>
#include <global/heap.h>

static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

volatile struct limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};

void memory_init(void) {
    hhdm_address = (void*)hhdm_request.response->offset;
    pmm_init();
    vmm_init();
    heap_init((void*)kernel_address_request.response->virtual_base, vmm_get_virtual_address((void*)highest_page_index));
}
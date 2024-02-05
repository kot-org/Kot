#include <impl/vmm.h>
#include <lib/math.h>
#include <global/pmm.h>
#include <boot/limine.h>

#include <arch/include.h>
#include ARCH_INCLUDE(vmm.h)

extern volatile struct limine_memmap_request memmap_request;
extern volatile struct limine_kernel_address_request kernel_address_request;

extern uint8_t kernel_start;
extern uint8_t kernel_end;

void vmm_init(void) {
    /* limine map all we need, so all we have to do is preload the empty fields (the higher half part of the table: VMM_HALF_TABLE - VMM_END_TABLE) into the last pagination level. */
    kernel_space = pmm_allocate_page();
    memset(vmm_get_virtual_address(kernel_space), 0, PAGE_SIZE);

    uintptr_t kernel_base = ALIGN_DOWN((uintptr_t)&kernel_start, PAGE_SIZE);
    uintptr_t kernel_top = ALIGN_UP((uintptr_t)&kernel_end, PAGE_SIZE);

    vmm_free_contiguous_address_iteration = (void*)kernel_top;
    
    uintptr_t physical_address = kernel_address_request.response->physical_base;
    for(uintptr_t virtual_address = kernel_base; virtual_address < kernel_top;){
        vmm_map_page(kernel_space, (void*)virtual_address, (void*)physical_address, MEMORY_FLAG_READABLE | MEMORY_FLAG_WRITABLE | MEMORY_FLAG_EXECUTABLE);
        virtual_address += PAGE_SIZE;
        physical_address += PAGE_SIZE;
    }

    for(uintptr_t address = 0x0; address < 0x100000000; address += PAGE_SIZE){
        vmm_map_page(kernel_space, vmm_get_virtual_address((void*)address), (void*)address, MEMORY_FLAG_READABLE | MEMORY_FLAG_WRITABLE);
    }

    struct limine_memmap_response* memmap = memmap_request.response;

    for(size_t i = 0; i < memmap->entry_count; i++){
        struct limine_memmap_entry* entry = memmap->entries[i];

        uintptr_t base = ALIGN_DOWN(entry->base, PAGE_SIZE);
        uintptr_t top = ALIGN_UP(entry->base + entry->length, PAGE_SIZE);

        if(top <= 0x100000000){
            continue;
        }

        for(uintptr_t address = base; address < top; address += PAGE_SIZE){
            if(address < 0x100000000){
                continue;
            }

            vmm_map_page(kernel_space, vmm_get_virtual_address((void*)address), (void*)address, MEMORY_FLAG_READABLE | MEMORY_FLAG_WRITABLE);
        }
    }

    vmm_preload_higher_half_entries(kernel_space);

    vmm_space_swap(kernel_space);
}

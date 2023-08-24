#include <global/pmm.h>
#include <boot/limine.h>

#include <lib/log.h>
#include <impl/vmm.h>
#include <lib/math.h>
#include <lib/assert.h>
#include <lib/bitmap.h>
#include <lib/memory.h>

#include <arch/include.h>
#include ARCH_INCLUDE(apic.h)

volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};


static void* pmm_get_memory_end(struct limine_memmap_response* memory_info) {
    uintptr_t last_address = 0;

    for(uint64_t i = 0; i < memory_info->entry_count; i++) {
        uintptr_t entry_last_address = memory_info->entries[i]->base + memory_info->entries[i]->length;
        if(entry_last_address > last_address) {
            last_address = entry_last_address;
        }
    }

    return (void*)last_address;
}

static size_t pmm_get_memory_size(struct limine_memmap_response* memory_info) {
    uint64_t size = 0;

    for(uint64_t i = 0; i < memory_info->entry_count; i++) {
        size += memory_info->entries[i]->length;
    }

    return (size_t)size;
}

static struct limine_memmap_entry* pmm_find_free_entry(struct limine_memmap_response* memory_info, size_t minimum_size) {
    for(uint64_t i = 0; i < memory_info->entry_count; i++) {
        if(memory_info->entries[i]->type == LIMINE_MEMMAP_USABLE) {
            if(memory_info->entries[i]->length >= minimum_size) {
                if(memory_info->entries[i]->base < TRAMPOLINE_END){
                    if(memory_info->entries[i]->length - (TRAMPOLINE_END - memory_info->entries[i]->base) >= minimum_size){
                        return memory_info->entries[i];
                    }
                }else{
                    return memory_info->entries[i];
                }
            }
        }
    }   
    return NULL;
}

void pmm_init(void) {
    struct limine_memmap_response* memory_info = memmap_request.response;

    void* memory_end = pmm_get_memory_end(memory_info);
    size_t memory_size = pmm_get_memory_size(memory_info);

    size_t total_page_count = DIV_ROUNDUP((size_t)memory_end, PAGE_SIZE);

    available_pages = 0;
    used_pages = 0;
    total_pages = (uint64_t)total_page_count;
    reserved_pages = (uint64_t)total_page_count;
    highest_page_index = (uint64_t)total_page_count;

    size_t bitmap_size = total_page_count / 8 + 1;
    struct limine_memmap_entry* bitmap_memmap_entry = pmm_find_free_entry(memory_info, bitmap_size);

    assert(bitmap_memmap_entry);

    void* bitmap_base;
    if(bitmap_memmap_entry->base < TRAMPOLINE_END){
        bitmap_base = (void*)TRAMPOLINE_END;
    }else{
        bitmap_base = (void*)bitmap_memmap_entry->base;
    }

    pmm_init_bitmap(vmm_get_virtual_address(bitmap_base), bitmap_size, true); /* Reserve all pages */

    for(uint64_t i = 0; i < memory_info->entry_count; i++) {
        if(memory_info->entries[i]->type == LIMINE_MEMMAP_USABLE) {
            uintptr_t base = (uintptr_t)memory_info->entries[i]->base;
            uint64_t size = memory_info->entries[i]->length;
            if(base <= TRAMPOLINE_END){
                size = size - (TRAMPOLINE_END - memory_info->entries[i]->base);
                base = TRAMPOLINE_END;
            }
            if(memory_info->entries[i] != bitmap_memmap_entry) {
                pmm_unreserve_pages((void*)base, size / PAGE_SIZE);
            }else{
                if(memory_info->entries[i]->length != bitmap_size) {
                    uintptr_t bitmap_end_rounded_to_page = base + bitmap_size;
                    
                    if(bitmap_end_rounded_to_page % PAGE_SIZE) {
                        bitmap_end_rounded_to_page -= bitmap_end_rounded_to_page % PAGE_SIZE;
                        bitmap_end_rounded_to_page += PAGE_SIZE;
                    }

                    pmm_unreserve_pages((void*)bitmap_end_rounded_to_page, (memory_info->entries[i]->length - bitmap_size) / PAGE_SIZE);
                }
            }
        }
    }
}

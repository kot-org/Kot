#ifndef _GLOBAL_PMM_H
#define _GLOBAL_PMM_H 1

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <arch/include.h>

/* 
The file in ARCH_INCLUDE(impl/vmm.h) is expected to have :
    - The definitation : PAGE_SIZE
*/
#include ARCH_INCLUDE(impl/pmm.h)

extern uint64_t available_pages;
extern uint64_t used_pages;
extern uint64_t reserved_pages;
extern uint64_t total_pages;

extern uint64_t highest_page_index;
extern uint64_t last_used_index;

void pmm_init(void);

void pmm_init_bitmap(void* address, size_t size, bool lock_all);

void* pmm_allocate_page(void);

void* pmm_allocate_pages(uint64_t page_count);

void pmm_free_page(void* address);

void pmm_free_pages(void* address, uint64_t page_count);

void pmm_lock_page(void* address);

void pmm_lock_pages(void* address, uint64_t page_count);

void pmm_unreserve_page(void* address);

void pmm_unreserve_pages(void* address, uint64_t page_count);

void pmm_reserve_page(void* address);

void pmm_reserve_pages(void* address, uint64_t page_count);

static inline uint64_t pmm_get_available(void) {
    return available_pages * PAGE_SIZE;
}

static inline uint64_t pmm_get_reserved(void) {
    return reserved_pages * PAGE_SIZE;
}

static inline uint64_t pmm_get_used(void) {
    return used_pages * PAGE_SIZE;
}

#endif // _GLOBAL_PMM_H

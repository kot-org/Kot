#ifndef _VMM_H
#define _VMM_H 1

#include <lib/memory.h>
#include <arch/include.h>

/* 
The file in ARCH_INCLUDE(impl/vmm.h) is expected to have :
    - The declaration of type : vmm_space_t
*/
#include ARCH_INCLUDE(impl/vmm.h)

extern void* hhdm_address;
extern void* vmm_free_contiguous_address_iteration;
extern vmm_space_t kernel_space;

void vmm_init(void);

vmm_space_t vmm_create_space(void);

vmm_space_t vmm_get_kernel_space(void);

vmm_space_t vmm_get_current_space(void);

int vmm_destroy_space(vmm_space_t space);

int vmm_space_swap(vmm_space_t space);

int vmm_map_page(vmm_space_t space, void* virtual_page, void* physical_page, memory_flags_t flags);

int vmm_map(vmm_space_t space, memory_range_t virtual_range, memory_range_t physical_range, memory_flags_t flags);

int vmm_update_flags(vmm_space_t space, memory_range_t virtual_range, memory_flags_t flags);

int vmm_unmap(vmm_space_t space, memory_range_t virtual_range);

int vmm_flush(vmm_space_t space, memory_range_t virtual_range);

void* vmm_get_physical_address(vmm_space_t space, void* virtual_address);

void* vmm_get_free_contiguous(size_t size);

static inline void* vmm_get_virtual_address(void* physical_address) {
    return (void*)((uintptr_t)physical_address + (uintptr_t)hhdm_address);
}

#endif // _VMM_H

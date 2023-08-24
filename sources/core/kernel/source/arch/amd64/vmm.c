#include <errno.h>
#include <impl/vmm.h>
#include <lib/lock.h>
#include <global/pmm.h>
#include <lib/assert.h>
#include <arch/include.h>
#include ARCH_INCLUDE(asm.h)


#define VMM_START_TABLE     0x0
#define VMM_HALF_TABLE      0x100
#define VMM_END_TABLE       0x200

#define VMM_FLAG_MASK               0xfff

#define VMM_FLAG_PRESENT            (1 << 0)
#define VMM_FLAG_READ_WRITE         (1 << 1)
#define VMM_FLAG_USER               (1 << 2)
#define VMM_FLAG_WRITE_THROUGH      (1 << 3)
#define VMM_FLAG_CACHE_DISABLED     (1 << 4)
#define VMM_FLAG_ACCESSED           (1 << 5)
#define VMM_FLAG_DIRTY              (1 << 6)
#define VMM_FLAG_LARGER_PAGES       (1 << 7)

#define VMM_GET_FLAG(entry, flag)                       ((uintptr_t)entry & flag)   
#define VMM_SET_FLAGS(entry, flags)                     entry = (vmm_entry)((uintptr_t)entry | (flags & VMM_FLAG_MASK))   
#define VMM_GET_PHYSICAL(entry)                         (void*)(((uintptr_t)entry) & 0x000ffffffffff000)   
#define VMM_SET_PHYSICAL(entry, physical_address)       entry = (vmm_entry)(((uintptr_t)physical_address & 0x000ffffffffff000) | entry)   

#define VMM_PML_X_GET_INDEX(address, level) (((uint64_t)address & ((uint64_t)0x1ff << (12 + level * 9))) >> (12 + level * 9))

#define VMM_PML_4_GET_INDEX(address) VMM_PML_X_GET_INDEX(address, 3)
#define VMM_PML_3_GET_INDEX(address) VMM_PML_X_GET_INDEX(address, 2)
#define VMM_PML_2_GET_INDEX(address) VMM_PML_X_GET_INDEX(address, 1)
#define VMM_PML_1_GET_INDEX(address) VMM_PML_X_GET_INDEX(address, 0)

void* hhdm_address = NULL;
vmm_space_t kernel_space = NULL;
void* vmm_free_contiguous_address_iteration = NULL;

typedef uint64_t vmm_entry;

struct vmm_page_table{ 
    vmm_entry entries[512];
}__attribute__((packed));

static vmm_entry vmm_make_entry(void* physical_address, memory_flags_t flags) {
    vmm_entry entry = 0;
    VMM_SET_PHYSICAL(entry, physical_address);
    VMM_SET_FLAGS(entry, 
        VMM_FLAG_PRESENT                                                        |
        ((MEMORY_FLAG_USER & flags)     ?   VMM_FLAG_USER               : 0)    |
        ((MEMORY_FLAG_WRITABLE & flags) ?   VMM_FLAG_READ_WRITE         : 0)    |
        ((MEMORY_FLAG_DMA & flags)      ?   VMM_FLAG_CACHE_DISABLED     : 0)
    );

    return entry;
}

static struct vmm_page_table* vmm_get_entry(struct vmm_page_table* table, size_t index, memory_flags_t flags) {
    vmm_entry entry = table->entries[index];

    if(VMM_GET_FLAG(entry, VMM_FLAG_PRESENT)) {
        return vmm_get_virtual_address(VMM_GET_PHYSICAL(entry));
    }else{
        void* physical_address = pmm_allocate_page();
        void* virtual_address = vmm_get_virtual_address(physical_address);
        memset(virtual_address, 0, PAGE_SIZE);
        table->entries[index] = vmm_make_entry(physical_address, flags);
        return (struct vmm_page_table*)virtual_address;
    }
}

static int vmm_update_page_flags(struct vmm_page_table* table, void* virtual_page, memory_flags_t flags) {
    struct vmm_page_table* pml4 = table;
    struct vmm_page_table* pml3 = vmm_get_entry(pml4, VMM_PML_4_GET_INDEX(virtual_page), flags | VMM_FLAG_READ_WRITE | MEMORY_FLAG_USER);
    struct vmm_page_table* pml2 = vmm_get_entry(pml3, VMM_PML_3_GET_INDEX(virtual_page), flags | VMM_FLAG_READ_WRITE | MEMORY_FLAG_USER);
    struct vmm_page_table* pml1 = vmm_get_entry(pml2, VMM_PML_2_GET_INDEX(virtual_page), flags | VMM_FLAG_READ_WRITE | MEMORY_FLAG_USER);
    
    vmm_entry* entry = &pml1->entries[VMM_PML_1_GET_INDEX(virtual_page)];

    if(!VMM_GET_FLAG(*entry, VMM_FLAG_PRESENT)) {
        return EADDRNOTAVAIL;
    }

    *entry = vmm_make_entry(VMM_GET_PHYSICAL(entry), flags);

    return 0;
}

static int vmm_unmap_page(struct vmm_page_table* table, void* virtual_page) {
    struct vmm_page_table* pml4 = table;
    struct vmm_page_table* pml3 = vmm_get_entry(pml4, VMM_PML_4_GET_INDEX(virtual_page), VMM_FLAG_READ_WRITE | MEMORY_FLAG_USER);
    struct vmm_page_table* pml2 = vmm_get_entry(pml3, VMM_PML_3_GET_INDEX(virtual_page), VMM_FLAG_READ_WRITE | MEMORY_FLAG_USER);
    struct vmm_page_table* pml1 = vmm_get_entry(pml2, VMM_PML_2_GET_INDEX(virtual_page), VMM_FLAG_READ_WRITE | MEMORY_FLAG_USER);
    
    vmm_entry* entry = &pml1->entries[VMM_PML_1_GET_INDEX(virtual_page)];

    if(!VMM_GET_FLAG(*entry, VMM_FLAG_PRESENT)) {
        return EADDRNOTAVAIL;
    }

    *entry = 0;

    return 0;
}

static int vmm_flush_page(void* virtual_page) {
    asm_invlpg(virtual_page);
    return 0;
}

vmm_space_t vmm_create_space(void) {
    vmm_space_t space = pmm_allocate_page();
    struct vmm_page_table* table = vmm_get_virtual_address(space);
    memset(table, 0, PAGE_SIZE);

    struct vmm_page_table* kernel_table = vmm_get_virtual_address(kernel_space);

    for(uint16_t i = VMM_HALF_TABLE; i < VMM_END_TABLE; i++){
        table->entries[i] = kernel_table->entries[i];
    }

    return space;
}

vmm_space_t vmm_get_kernel_space(void) {
    return kernel_space;
}

vmm_space_t vmm_get_current_space(void) {
    return asm_get_paging_entry();
}

int vmm_destroy_space(vmm_space_t space) {
    if(space == kernel_space) {
        return EACCES;
    }

    // TODO

    return 0;
}

int vmm_space_swap(vmm_space_t space) {
    asm_set_paging_entry(space);
    return 0;
}

int vmm_map_page(vmm_space_t space, void* virtual_page, void* physical_page, memory_flags_t flags) {
    struct vmm_page_table* pml4 = vmm_get_virtual_address(space);
    struct vmm_page_table* pml3 = vmm_get_entry(pml4, VMM_PML_4_GET_INDEX(virtual_page), flags | VMM_FLAG_READ_WRITE | MEMORY_FLAG_USER);
    struct vmm_page_table* pml2 = vmm_get_entry(pml3, VMM_PML_3_GET_INDEX(virtual_page), flags | VMM_FLAG_READ_WRITE | MEMORY_FLAG_USER);
    struct vmm_page_table* pml1 = vmm_get_entry(pml2, VMM_PML_2_GET_INDEX(virtual_page), flags | VMM_FLAG_READ_WRITE | MEMORY_FLAG_USER);
    
    vmm_entry* entry = &pml1->entries[VMM_PML_1_GET_INDEX(virtual_page)];
    
    *entry = vmm_make_entry(physical_page, flags);

    return 0;
}

int vmm_map(vmm_space_t space, memory_range_t virtual_range, memory_range_t physical_range, memory_flags_t flags) {
    if(virtual_range.size != physical_range.size) {
        return EINVAL;
    }

    for(uintptr_t i = 0; i < virtual_range.size; i += PAGE_SIZE) {
        vmm_map_page(space, (void*)((uintptr_t)virtual_range.address + i), (void*)((uintptr_t)physical_range.address + i), flags);
    }

    return 0;
}

int vmm_update_flags(vmm_space_t space, memory_range_t virtual_range, memory_flags_t flags) {
    struct vmm_page_table* table = vmm_get_virtual_address(space);

    for(uintptr_t i = 0; i < virtual_range.size; i += PAGE_SIZE) {
        assert(!vmm_update_page_flags(table, (void*)((uintptr_t)virtual_range.address + i), flags));
    }

    return 0;    
}

int vmm_unmap(vmm_space_t space, memory_range_t virtual_range) {
    struct vmm_page_table* table = vmm_get_virtual_address(space);

    for(uintptr_t i = 0; i < virtual_range.size; i += PAGE_SIZE) {
        assert(!vmm_unmap_page(table, (void*)((uintptr_t)virtual_range.address + i)));
    }

    return 0;
}

int vmm_flush(vmm_space_t space, memory_range_t virtual_range) {
    if(space != vmm_get_current_space()) {
        return EINVAL;
    }

    for(uintptr_t i = 0; i < virtual_range.size; i += PAGE_SIZE) {
        assert(!vmm_flush_page((void*)((uintptr_t)virtual_range.address + i)));
    }

    return 0;
}

void* vmm_get_physical_address(vmm_space_t space, void* virtual_address) {
    struct vmm_page_table* pml4 = (struct vmm_page_table*)vmm_get_virtual_address(space);
    struct vmm_page_table* pml3 = vmm_get_entry(pml4, VMM_PML_4_GET_INDEX(virtual_address), VMM_FLAG_READ_WRITE | MEMORY_FLAG_USER);
    struct vmm_page_table* pml2 = vmm_get_entry(pml3, VMM_PML_3_GET_INDEX(virtual_address), VMM_FLAG_READ_WRITE | MEMORY_FLAG_USER);
    struct vmm_page_table* pml1 = vmm_get_entry(pml2, VMM_PML_2_GET_INDEX(virtual_address), VMM_FLAG_READ_WRITE | MEMORY_FLAG_USER);
    
    vmm_entry* entry = &pml1->entries[VMM_PML_1_GET_INDEX(virtual_address)];

    if(!VMM_GET_FLAG(*entry, VMM_FLAG_PRESENT)) {
        return NULL;
    }

    return (void*)((uintptr_t)VMM_GET_PHYSICAL(*entry) + ((uintptr_t)virtual_address % PAGE_SIZE));
}

int vmm_clear_lower_half_entries(vmm_space_t space) {
    struct vmm_page_table* last_level_table = (struct vmm_page_table*)vmm_get_virtual_address(space);
    
    for(uint16_t i = VMM_START_TABLE; i < VMM_HALF_TABLE; i++) {
        last_level_table->entries[i] = 0;
    }

    return 0;
}

int vmm_preload_higher_half_entries(vmm_space_t space) {
    struct vmm_page_table* last_level_table = (struct vmm_page_table*)vmm_get_virtual_address(space);
    
    for(uint16_t i = VMM_HALF_TABLE; i < VMM_END_TABLE; i++) {
        vmm_entry* entry = &last_level_table->entries[i];

        if(!VMM_GET_FLAG(*entry, VMM_FLAG_PRESENT)) {
            void* physical_address = pmm_allocate_page();
            void* virtual_address = vmm_get_virtual_address(physical_address);
            memset(virtual_address, 0, PAGE_SIZE);
            
            *entry = vmm_make_entry(physical_address, MEMORY_FLAG_READABLE | MEMORY_FLAG_WRITABLE | MEMORY_FLAG_EXECUTABLE | MEMORY_FLAG_USER);
        }
    }

    return 0;
}

static spinlock_t get_free_contiguous_lock = {};

void* vmm_get_free_contiguous(size_t size){
    spinlock_acquire(&get_free_contiguous_lock);

    if(size % PAGE_SIZE){
        size -= size % PAGE_SIZE;
        size += PAGE_SIZE;
    }

    void* return_value = vmm_free_contiguous_address_iteration;

    vmm_free_contiguous_address_iteration = (void*)((uintptr_t)vmm_free_contiguous_address_iteration + (uintptr_t)size);

    spinlock_release(&get_free_contiguous_lock);

    for(size_t i = 0; i < size; i += PAGE_SIZE){
        vmm_map_page(kernel_space, (void*)((uintptr_t)return_value + (uintptr_t)i), pmm_allocate_page(), MEMORY_FLAG_READABLE | MEMORY_FLAG_WRITABLE | MEMORY_FLAG_EXECUTABLE);
    }

    return return_value;
}
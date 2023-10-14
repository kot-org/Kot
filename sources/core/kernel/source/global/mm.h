#ifndef _GLOBAL_MM_H
#define _GLOBAL_MM_H 1

#include <errno.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/mman.h>

typedef struct{
    void* base;
    size_t size;
    uint64_t region_count;
    struct memory_region_t* first_region;
    struct memory_region_t* last_free_region;
    vmm_space_t vmm_space;
    uint64_t memory_allocated;
    spinlock_t lock;
} memory_handler_t;


typedef struct memory_region_t{
    void* base;
    size_t size;
    uint64_t block_count;
    bool is_free;

    struct memory_region_t* next;
    struct memory_region_t* last;
} memory_region_t;


memory_handler_t* mm_create_handler(vmm_space_t vmm_space, void* base, size_t size);
memory_handler_t* mm_clone_handler(vmm_space_t vmm_space, memory_handler_t* source);
int mm_free_handler(memory_handler_t* handler);

int mm_allocate_region_vm(memory_handler_t* handler, void* base, size_t size, bool is_fixed, void** base_result);
int mm_free_region(memory_handler_t* handler, void* base, size_t size);
memory_region_t* mm_get_region(memory_handler_t* handler, void* base);

int mm_allocate_memory_block(memory_handler_t* handler, void* base, size_t size, int prot, size_t* size_allocate);
int mm_allocate_memory_contigous(memory_handler_t* handler, void* base, size_t size, int prot, size_t* size_allocate);
int mm_map_physical(memory_handler_t* handler, void* base_physical, void* base, size_t size, int prot);

int mm_share_region(memory_handler_t* handler, vmm_space_t space, void* base_dst, void* base_src, size_t size, int prot);

int mm_unmap(memory_handler_t* handler, void* base, size_t size);

int mm_protect(memory_handler_t* handler, void* base, size_t size, int prot);

#endif // _GLOBAL_MM_H
#include <impl/vmm.h>
#include <impl/arch.h>
#include <lib/string.h>
#include <global/pmm.h>
#include <lib/printf.h>
#include <global/heap.h>

#include <arch/include.h>
#include ARCH_INCLUDE(asm.h)

#include "../odi-light/src/deps.h"

uint64_t ODI_DEP_PAGE_SIZE = PAGE_SIZE;
uint8_t ODI_DEP_MPROTECT_PAGE_CACHE_DISABLE = 1 << 1;
uint8_t ODI_DEP_MPROTECT_PAGE_WRITE_BIT = 1 << 4; 

//Conversion management.
char* odi_dep_itoa(uint64_t value, char* str, int base){
    return itoa(value, str, base);
}

uint64_t odi_dep_atoi(const char * str){
    return atoi(str);
}

int odi_dep_memcmp(const void *s1, const void *s2, size_t n){
    return memcmp(s1, s2, n);
}

void* odi_dep_memset(void *s, int c, size_t n){
    return memset(s, c, n);
}

void* odi_dep_memcpy(void *dest, const void *src, size_t n){
    return memcpy(dest, src, n);
}

//String management.
int odi_dep_strcmp(const char* str1, const char* str2){
    return strcmp(str1, str2);
}

int odi_dep_strncmp(const char* str1, const char* str2, int n){
    return strncmp(str1, str2, n);
}

int odi_dep_strlen(const char* str){
    return strlen(str);
}

char* odi_dep_strncpy(char *dest, const char *src, int n){
    return strncpy(dest, src, n);
}

//Heap management.
void* odi_dep_malloc(int size){
    return malloc(size);
}

void odi_dep_free(void* ptr){
    free(ptr);
}

//Virtual memory management.
void* odi_dep_get_free_contiguous_virtual_address(size_t size){
    vmm_get_free_contiguous(size);
}

void odi_dep_map_current_memory_size(void* virtual_address, void* physical_memory, size_t size){
    vmm_map(vmm_get_kernel_space(), (memory_range_t){virtual_address, size}, (memory_range_t){physical_memory, size}, MEMORY_FLAG_READABLE | MEMORY_FLAG_EXECUTABLE | MEMORY_FLAG_DMA);
}

void odi_dep_map_current_memory(void* virtual_memory, void* physical_memory){
    odi_dep_map_current_memory_size(virtual_memory, physical_memory, PAGE_SIZE);
}

void odi_dep_mprotect_current(void* address, uint64_t size, uint8_t permissions){
    vmm_update_flags(vmm_get_kernel_space(), (memory_range_t){address, size}, permissions);
}

void* odi_dep_request_page(void){
    return pmm_allocate_page();
}

void* odi_dep_get_virtual_address(void* address){
    return vmm_get_virtual_address(address);
}


//Print management.
void odi_dep_vprintf(const char* format, odi_va_list args){
    vprintf_(format, args);
}

void odi_dep_vsnprintf(char* buffer, int size, const char* format, odi_va_list args){
    vsnprintf_(buffer, size, format, args);
}

void odi_dep_printf(const char* format, ...){
    va_list args;
    va_start(args, format);
    log_printv(format, args);
    va_end(args);
}
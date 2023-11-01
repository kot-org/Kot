#include <impl/vmm.h>
#include <impl/arch.h>
#include <lib/string.h>
#include <global/pmm.h>
#include <lib/printf.h>
#include <global/heap.h>

#include <arch/include.h>
#include ARCH_INCLUDE(asm.h)

#include "../odi/src/deps.h"

//Conversion management.
char* odi_dep_itoa(s64 value, char* str, int base){
    return itoa(value, str, base);
}

s64 odi_dep_atoi(const char * str){
    return atoi(str);
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
    vmm_map(vmm_get_current_space(), (memory_range_t){virtual_address, size}, (memory_range_t){physical_memory, size}, MEMORY_FLAG_READABLE | MEMORY_FLAG_EXECUTABLE | MEMORY_FLAG_DMA);
}

void odi_dep_map_current_memory(void* virtual_memory, void* physical_memory){
    odi_dep_map_current_memory_size(virtual_memory, physical_memory, PAGE_SIZE);
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
    printf_(format, args);
    va_end(args);
}

//IO Management. Usually an embedded assembly line does the trick.
void odi_dep_outb(u16 port, u8 value){
    io_write8(port, value);
}

void odi_dep_outw(u16 port, u16 value){
    io_write16(port, value);
}

void odi_dep_outl(u16 port, u32 value){
    io_write32(port, value);
}

u8 odi_dep_inb(u16 port){
    return io_read8(port);
}

u16 odi_dep_inw(u16 port){
    return io_read16(port);
}

u32 odi_dep_inl(u16 port){
    return io_read32(port);
}

void odi_dep_insw(u16 port, u8* buffer, int count){
    io_read_string(port, buffer, count);
}

void odi_dep_outsw(u16 port, u8 *buffer, int count){
    io_write_string(port, buffer, count);
}

void odi_dep_io_wait(void){
    io_write8(0x80, 0);
}

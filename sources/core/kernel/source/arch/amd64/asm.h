#ifndef _AMD64_ASM_H
#define _AMD64_ASM_H

#include <stdint.h>
#include <impl/vmm.h>

static inline uint64_t asm_read_cr0(void) {  
    uint64_t value = 0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(value));
    return value;
} 
  
static inline void asm_write_cr0(uint64_t value) { 
    __asm__ volatile("mov %0, %%cr0" ::"r"(value));  
}

static inline uint64_t asm_read_cr2(void) {  
    uint64_t value = 0;
    __asm__ volatile("mov %%cr2, %0" : "=r"(value));
    return value;
} 

static inline uint64_t asm_read_cr3(void) {  
    uint64_t value = 0;
    __asm__ volatile("mov %%cr3, %0" : "=r"(value));
    return value;
} 
    
static inline void asm_write_cr3(uint64_t value) {
    __asm__ volatile("mov %0, %%cr3" ::"r"(value)); 
}

static inline vmm_space_t asm_get_paging_entry(void) {
    return (vmm_space_t)(asm_read_cr3() & ~(0xfff));
} 

static inline void asm_set_paging_entry(vmm_space_t space) {
    asm_write_cr3((((uint64_t)space) & ~(0xfff)) | (asm_read_cr3() & 0xfff));
} 

static inline uint64_t asm_read_cr4(void) {  
    uint64_t value = 0;
    __asm__ volatile("mov %%cr4, %0" : "=r"(value));
    return value;
} 
  
static inline void asm_write_cr4(uint64_t value) { 
    __asm__ volatile("mov %0, %%cr4" ::"r"(value));  
}

static inline void asm_fninit(void) { 
    __asm__ volatile("fninit");  
}

static inline void asm_invlpg(void* address) {
   __asm__ volatile("invlpg (%0)" ::"r" (address) : "memory");
}

static inline void io_write8(uint16_t port, uint8_t data) {
    __asm__ volatile("outb %b0, %w1" : : "a" (data), "d" (port));
}

static inline void io_write16(uint16_t port, uint16_t data) {
    __asm__ volatile("outw %w0, %w1" : : "a" (data), "d" (port));
}

static inline void io_write32(uint16_t port, uint32_t data) {
	__asm__ volatile("outl %0, %w1" : : "a" (data), "d" (port));
}

static inline uint8_t io_read8(uint16_t port) {
    uint8_t data;
    __asm__ volatile("inb %w1, %b0" : "=a" (data) : "d" (port));
    return data;
}

static inline uint16_t io_read16(uint16_t port) {
    uint16_t data;
    __asm__ volatile("inw %w1, %w0" : "=a" (data) : "d" (port));
    return data;
}

static inline uint32_t io_read32(uint16_t port) {
    uint32_t data;
    __asm__ volatile("inl %w1, %0" : "=a" (data) : "d" (port));
    return data;
}

static inline uint64_t rdmsr(uint32_t index){
    uint32_t lower;
    uint32_t upper;
    asm volatile("rdmsr" : "=a"(lower), "=d"(upper) : "c"(index));
    return ((uint64_t) upper << 32) | lower;
}

static inline void wrmsr(uint32_t index, uint64_t value){
    uint32_t lower = (uint32_t) value;
    uint32_t upper = (uint32_t) (value >> 32);
    asm volatile("wrmsr" :: "a"(lower), "c"(index), "d"(upper));
}

#endif // _AMD64_ASM_H

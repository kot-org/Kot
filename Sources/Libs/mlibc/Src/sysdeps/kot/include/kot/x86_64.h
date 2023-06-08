#ifndef KOT_X86_64_H
#define KOT_X86_64_H 1

#include <kot/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

static inline void kot_IoWrite8(uint16_t port, uint8_t data){
    __asm__ volatile("outb %b0, %w1" : : "a" (data), "Nd" (port));
}

static inline void kot_IoWrite16(uint16_t port, uint16_t data){
    __asm__ volatile("outw %w0, %w1" : : "a" (data), "Nd" (port));
}

static inline void kot_IoWrite32(uint16_t port, uint32_t data){
	__asm__ volatile("outl %0, %w1" : : "a" (data), "Nd" (port));
}

static inline uint8_t kot_IoRead8(uint16_t port){
    uint8_t data;
    __asm__ volatile("inb %w1, %b0" : "=a" (data) : "Nd" (port));
    return data;
}

static inline uint16_t kot_IoRead16(uint16_t port){
    uint16_t data;
    __asm__ volatile("inw %w1, %w0" : "=a" (data) : "Nd" (port));
    return data;
}

static inline uint32_t kot_IoRead32(uint16_t port){
    uint32_t data;
    __asm__ volatile("inl %w1, %0" : "=a" (data) : "Nd" (port));
    return data;
}

// mmio

static inline void kot_MmioWrite8(void* address, uint8_t value) {
    *(volatile uint8_t*) address = value;
}

static inline void kot_MmioWrite16(void* address, uint16_t value) {
    *(volatile uint16_t*) address = value;
}

static inline void kot_MmioWrite32(void* address, uint32_t value) {
    *(volatile uint32_t*) address = value;
}

static inline void kot_MmioWrite64(void* address, uint64_t value) {
    *(volatile uint64_t*) address = value;
}

static inline uint8_t kot_MmioRead8(void* address) {
    return *(volatile uint8_t*) address;
}

static inline uint16_t kot_MmioRead16(void* address) {
    return *(volatile uint16_t*) address;
}

static inline uint32_t kot_MmioRead32(void* address) {
    return *(volatile uint32_t*) address;
}

static inline uint64_t kot_MmioRead64(void* address) {
    return *(volatile uint64_t*) address;
}

#if defined(__cplusplus)
} 
#endif

#endif 
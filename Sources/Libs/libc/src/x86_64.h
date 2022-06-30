#ifndef _X86_64_H
#define _X86_64_H 1

#include <kot/types.h>

static inline void IoWrite8(uint16_t port, uint8_t data){
    __asm__ volatile("outb %b0, %w1" : : "a" (data), "Nd" (port));
}

static inline void IoWrite16(uint16_t port, uint16_t data){
    __asm__ volatile("outw %w0, %w1" : : "a" (data), "Nd" (port));
}

static inline void IoWrite32(uint16_t port, uint32_t data){
	__asm__ volatile("outl %0, %w1" : : "a" (data), "Nd" (port));
}

static inline uint8_t IoRead8(uint16_t port){
    uint8_t data;
    __asm__ volatile("inb %w1, %b0" : "=a" (data) : "Nd" (port));
    return data;
}

static inline uint16_t IoRead16(uint16_t port){
    uint16_t data;
    __asm__ volatile("inw %w1, %w0" : "=a" (data) : "Nd" (port));
    return data;
}

static inline uint32_t IoRead32(uint16_t port){
    uint32_t data;
    __asm__ volatile("inl %w1, %0" : "=a" (data) : "Nd" (port));
    return data;
}

#endif 
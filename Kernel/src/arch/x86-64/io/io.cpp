#include "io.h"

bool IO_IN(uint8_t size, uint16_t port, uint32_t data){
    switch(size){
        case 8:
            IoWrite8(port, (uint8_t)data);
            return true;
        case 16:
            IoWrite16(port, (uint16_t)data);
            return true;
        case 32:
            IoWrite32(port, (uint32_t)data);
            return true;
        default:
            return false;
    }
}

uint32_t IO_OUT(uint8_t size, uint16_t port){
    switch(size){
        case 8:
            return (uint32_t)IoRead8(port);
        case 16:
            return (uint32_t)IoRead16(port);
        case 32:
            return (uint32_t)IoRead32(port);
        default:
            return 0;
    }  
}

void IoWrite8(uint16_t port, uint8_t data){
    __asm__ volatile("outb %b0, %w1" : : "a" (data), "Nd" (port));
}

void IoWrite16(uint16_t port, uint16_t data){
    __asm__ volatile("outw %w0, %w1" : : "a" (data), "Nd" (port));
}

void IoWrite32(uint16_t port, uint32_t data){
	__asm__ volatile("outl %0, %w1" : : "a" (data), "Nd" (port));
}

uint8_t IoRead8(uint16_t port){
    uint8_t data;
    __asm__ volatile("inb %w1, %b0" : "=a" (data) : "Nd" (port));
    return data;
}

uint16_t IoRead16(uint16_t port){
    uint16_t data;
    __asm__ volatile("inw %w1, %w0" : "=a" (data) : "Nd" (port));
    return data;
}

uint32_t IoRead32(uint16_t port){
    uint32_t data;
    __asm__ volatile("inl %w1, %0" : "=a" (data) : "Nd" (port));
    return data;
}

void io_wait(){
    asm volatile ("outb %%al, $0x80" : : "a"(0));
}
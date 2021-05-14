#ifndef _IDT_H
#define _IDT_H
#include <sys/defs.h>
//defining IDT entry
struct idt_entry{
uint16_t base_lo;
uint16_t sel;
uint8_t always0;
uint8_t  flags;
uint16_t base_mid;
uint32_t base_hi;
uint32_t  ist;
}__attribute__((packed));

struct idt_ptr{
uint16_t limit;
uint64_t base;
}__attribute__((packed));

//function to load interrupt descriptor table 
extern void idt_load();

//function to set IDT entries
void idt_set_gate(uint16_t num, uint64_t base,uint16_t sel, uint8_t flags);
//void idt_set_gate(unsigned char num, unsigned long base,unsigned short sel, unsigned char flags);

//install function
void idt_install();
void init_isr();

void outb(int16_t port, uint16_t val);
void PIC_remap();
//void* memsett(void *str,int val,uint64_t size);

//static __inline uint64_t syscall_3(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3);

#endif

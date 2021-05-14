#include "ports.h"

unsigned char inportb(unsigned int port)
{
   unsigned char ret;
   __asm__ volatile ("inb %%dx,%%al":"=a" (ret):"d" (port));
   return ret;
}

unsigned long long inportl(unsigned long long port)
{
   unsigned long long ret;
   __asm__ volatile ("inl %%dx,%%eax":"=a" (ret):"d" (port));
   return ret;
}

void outportb(unsigned int port,unsigned char value)
{
   __asm__ volatile ("outb %%al,%%dx": :"d" (port), "a" (value));
}

void outportw(unsigned short int port, unsigned short int data) {
	__asm__ volatile("outw %%ax, %%dx" : : "d"(port), "a"(data));
}

void outportl(unsigned long long port, unsigned long long data) {
   __asm__ volatile("outl %%eax, %%dx" : : "d"(port), "a"(data));
}

void io_wait() {
   asm volatile ("outb %%al, $0x80" : : "a"(0));
}
/*#include <sys/irq.h>
#include <sys/defs.h>
#include <sys/idt.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/timer.h>
 	
void *irq [16] =
	{ 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0
	};

char inb(uint16_t port) {
        char ret;
        __asm__ volatile ("inb %%dx,%%al":"=a"(ret):"d"(port));
       return ret;
}
void outb(int16_t port, uint16_t val)
{
        __asm__ volatile ("outb %%al, %%dx"::"d" (port), "a" (val));

}

void irq_install(){
//PIC_remap();
kprintf("irq %p", irq);
idt_set_gate(32, (uint64_t)&handler_timer, 0x08, 0x8E);
//idt_set_gate(33, (uint64_t)handler_keyboard,0x08, 0x8E);


irq_remap();
timer_install();
//handler_timer();

}

void irq_hander_install(int req, void(* handler)(void)){
    irq[req] = handler;
}

void irq_hander_uninstall(int req){
    irq[req]=0; 			 
}

//remapping IRQ0 to 15 to IDT entries 32 to 47 so that IRQ uses non-reserved vectors
void irq_remap(void)
{
    outb(0x20, 0x11); // master PIc- command i/o port
    outb(0xA0, 0x11); // slave -command
    outb(0x21, 0x20);  // master -data(beginning)
    outb(0xA1, 0x28);  // slave- data
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}*/

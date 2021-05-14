#include <sys/gdt.h>
#include <sys/idt.h>
#include <sys/defs.h>
#include <sys/timer.h>
#include <sys/kprintf.h>
#include <sys/interrupt.h>
#include <sys/syscall.h>
#include <sys/pmap.h>
#include <sys/vmmu.h>
#include <sys/string.h>

static struct idt_entry idt[256];
static struct idt_ptr idtp;
static unsigned char* v_ptr;

extern void handler_timer();
extern void handler_keyboard();
extern void handler_syscall();
extern void handler_page_fault();

void outb(int16_t port, uint16_t val)
{
        __asm__ volatile ("outb %%al, %%dx"::"d" (port), "a" (val));

}

void* memsett(void *str, int val, int num){
    char *temp = str;
    while(num-- > 0){
         *temp++ = val;
        }
    return str;
}

//function to load interrupt descriptor table 
void idt_install(){

     // v_ptr =(unsigned char*) 0xB8000 +(2*80*24+53*2);
      v_ptr =(unsigned char *)0xFFFFFFFF800B8000 +(2*80*24+53*2);
      char* str= "Timer :";
      while(*str!=0){
      *v_ptr = *str++; 	
       v_ptr = v_ptr +2;
       }	
	

//  idt[32].base_hi = ((uint64_t)handler_timer >>32)& 0xFFFFFFFF;
//  idt[32].base_lo = (uint64_t)handler_timer & 0xFFFF;
//  idt[32].base_mid = ( (uint64_t)handler_timer >>16) & 0xFFFF;
//  idt[32].sel = 0x08;
//  idt[32].always0 =0;
//  idt[32].flags=0x8E;
//  idt[32].ist =0;

  //sets value of the idt_pointer-- its base and limit
  idtp.limit = (uint64_t)(sizeof(struct idt_entry)*256 - 1);
  idtp.base  = (uint64_t)&idt;
 
  //clearing out idt pointers
  memsett(idt, 0, sizeof(struct idt_entry)*256);
  PIC_remap();
  idt_set_gate(32, (uint64_t)handler_timer, 0x08, 0x8E);
  idt_set_gate(33, (uint64_t)handler_keyboard, 0x08, 0x8E);
  idt_set_gate(14, (uint64_t)handler_page_fault, 0x08, 0x8E); 
  idt_set_gate(128, (uint64_t) handler_syscall, 0x08, 0xEE);
    
  
//adding new ISRs to IDT using set gate
  //init_isr();

//point processor's registers to IDT
//idt_load();
  __asm  volatile("lidt (%0)"::"r"(&idtp));
//  __asm ("sti");
}

//function to set IDT entries--accepts the IDT entry number, the base address of ISR, kernel code segment, and the access flags
	void idt_set_gate(uint16_t num, uint64_t base,uint16_t sel, uint8_t flags){
	idt[num].base_hi = (base >>32)& 0xFFFFFFFF;
	idt[num].base_lo = base& 0xFFFF;
	idt[num].base_mid = (base >>16) & 0xFFFF;
	idt[num].sel = sel;
	idt[num].always0 =0;
	idt[num].flags=flags;
	idt[num].ist =0; 
}

void PIC_remap(){
    kprintf("Reiniting PICS");
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

}


void register_interrupt_handler(registers_t *reg){
//	kprintf("\nregister_interrupt_handler");
	syscall_handler(reg);
	outb(0x20, 0x20);
}



/*
void page_fault_handler(registers_t *reg){
	uint64_t fault_addr;
 	uint64_t p_error = reg->err_code & PF_P;
	uint64_t r_error = reg->err_code & PF_W;
//	uint64_t u_error = reg->err_code & PF_U;  
	__asm volatile("mov %%cr2, %0" : "=r" (fault_addr));
	kprintf("\nYou encountered a page fault at address %x", fault_addr);
	//cow page fault handling
	if(p_error || r_error){
	
	}
	
	//system call for user

}
*/


void page_fault_handler(registers_t *reg)
{
//	kprintf("In Pg fault");
        volatile uint64_t faultAddr;
        __asm volatile("mov %%cr2, %0" : "=r" (faultAddr));
         uint64_t pte_entry = address_physical(faultAddr);
                if(pte_entry & PTE_COW) {
                        uint64_t temp_vaddr = (uint64_t)kmalloc(4096);
                        uint64_t paddr = address_physical(get_address(&temp_vaddr));
                        faultAddr = ((faultAddr / 4096) * 4096);
                        memcpy((void *)temp_vaddr, (void *)(faultAddr), 4096);
                        map_process(faultAddr, paddr);
                        pte_entry = (pte_entry & ~0xfff)| PTE_P | PTE_W | PTE_U;
                        return;
                }
}





/*static __inline uint64_t syscall_3(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3) {
	uint64_t a; 
    __asm__ volatile("int $0x80" : "=a" (a) : "0" (n), "D" ((uint64_t)a1), "S" ((uint64_t)a2), "b"((uint64_t)a3)); 
    return a;


}*/	

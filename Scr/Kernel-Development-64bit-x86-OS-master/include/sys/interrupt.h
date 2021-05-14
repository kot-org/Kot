#ifndef _INTERRUPT_H
#define _INTERRUPT_H
#include <sys/defs.h>

#define PF_P	0x1
#define PF_W	0x2
#define PF_U	0x4

typedef struct registers
{
/*    uint64_t ds;                  // Data segment selector
    uint64_t rdi, rsi, rbp, rsp, rbx, rdx, rcx, rax; // Pushed by pusha.
    uint64_t int_no, err_code;    // Interrupt number and error code (if applicable)
    uint64_t rip, cs, rflags, userrsp, ss; // Pushed by the processor automatically.*/
	uint64_t r9;
	uint64_t r8;

	uint64_t rsi;
	uint64_t rbp;
	uint64_t rdx;
	uint64_t rcx;
	uint64_t rbx;
	uint64_t rax;
	uint64_t rdi;

	uint64_t int_no;
	uint64_t err_code;

	uint64_t rip;
    	uint64_t cs;
    	uint64_t rflags;
    	uint64_t rsp;
    	uint64_t ss;
} registers_t;


extern void register_interrupt_handler(registers_t *reg);
extern void page_fault_handler(registers_t *reg);
#endif

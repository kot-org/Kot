#ifndef _GLOBAL_HW_INTERRUPT_H
#define _GLOBAL_HW_INTERRUPT_H 1

#include <impl/arch.h>
#include <impl/context.h>

typedef int (*hw_interrupt_handler_t)(int id);

int hw_interrupt_init(int count);
int hw_interrupt_trigger(cpu_context_t* ctx, int id);
int hw_interrupt_set_handler(int id, hw_interrupt_handler_t handler);

#endif // _GLOBAL_HW_INTERRUPT_H
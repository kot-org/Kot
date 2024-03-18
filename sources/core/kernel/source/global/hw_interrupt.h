#ifndef _GLOBAL_HW_INTERRUPT_H
#define _GLOBAL_HW_INTERRUPT_H 1

#include <impl/arch.h>
#include <impl/context.h>

typedef int (*hw_interrupt_handler_ptr_t)(int id);

typedef struct{
    hw_interrupt_handler_ptr_t handler_ptr;
    bool is_locked;
}hw_interrupt_handler_t;

int hw_interrupt_init(int count);
void hw_interrupt_lock(int id);
void hw_interrupt_unlock(int id);
int hw_interrupt_allocate(void);
int hw_interrupt_trigger(cpu_context_t* ctx, int id);
int hw_interrupt_set_handler(int id, hw_interrupt_handler_ptr_t handler);

#endif // _GLOBAL_HW_INTERRUPT_H
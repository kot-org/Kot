#include <stdlib.h>
#include <lib/assert.h>
#include <impl/hw_interrupt.h>
#include <global/hw_interrupt.h>

int handlers_count = 0;
hw_interrupt_handler_t* handlers = NULL;
spinlock_t hw_interrupt_allocate_lock = (spinlock_t)SPINLOCK_INIT;

int hw_interrupt_init(int count){
    handlers = calloc(count, sizeof(hw_interrupt_handler_t));
    handlers_count = count;
    return 0;
}

void hw_interrupt_lock(int id){
    assert(id >= 0);
    assert(id < handlers_count);

    handlers[id].is_locked = true; 
}

void hw_interrupt_unlock(int id){
    assert(id >= 0);
    assert(id < handlers_count);

    handlers[id].is_locked = false; 
}

int hw_interrupt_allocate(void){
    spinlock_acquire(&hw_interrupt_allocate_lock);
    for(int i = 0; i < handlers_count; i++){
        if(!handlers[i].is_locked){
            hw_interrupt_lock(i);
            spinlock_release(&hw_interrupt_allocate_lock);
            return i;
        }
    }
    spinlock_release(&hw_interrupt_allocate_lock);
    return -1;
}

int hw_interrupt_trigger(cpu_context_t* ctx, int id){
    if(id < 0){
        return EINVAL;
    }

    if(id < handlers_count){
        if(handlers[id].handler_ptr){
            return handlers[id].handler_ptr(id);
        }
    }

    return EINVAL;
}

int hw_interrupt_set_handler(int id, hw_interrupt_handler_ptr_t handler){
    if(id < 0){
        return EINVAL;
    }

    if(id < handlers_count){
        int error = hw_interrupt_set_state(id, true);
        if(error) return error;
        handlers[id].handler_ptr = handler;
        return 0;
    }

    return EINVAL;    
}
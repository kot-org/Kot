#include <stdlib.h>
#include <impl/hw_interrupt.h>
#include <global/hw_interrupt.h>

int handlers_count = 0;
hw_interrupt_handler_t* handlers = NULL;

int hw_interrupt_init(int count){
    handlers = calloc(count, sizeof(hw_interrupt_handler_t));
    handlers_count = count;
    return 0;
}

int hw_interrupt_trigger(cpu_context_t* ctx, int id){
    if(id < 0){
        return EINVAL;
    }

    if(id < handlers_count){
        if(handlers[id]){
            return handlers[id](id);
        }
    }

    return EINVAL;
}

int hw_interrupt_set_handler(int id, hw_interrupt_handler_t handler){
    if(id < 0){
        return EINVAL;
    }

    if(id < handlers_count){
        int error = hw_interrupt_set_state(id, true);
        if(error) return error;
        handlers[id] = handler;
        return 0;
    }

    return EINVAL;    
}
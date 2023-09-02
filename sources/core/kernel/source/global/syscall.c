#include <errno.h>
#include <stddef.h>
#include <lib/log.h>
#include <impl/arch.h>
#include <kot/syscall.h>
#include <global/syscall.h>


static void syscall_handler_log(cpu_context_t* ctx){
    char* message = (char*)ARCH_CONTEXT_ARG0(ctx);
    size_t len = (size_t)ARCH_CONTEXT_ARG1(ctx);

    log_printf("%.*s\n", len, message);

    ARCH_CONTEXT_RETURN(ctx) = 0;
}

static syscall_handler_t handlers[SYSCALL_COUNT] = { 
    syscall_handler_log
};

void syscall_handler(cpu_context_t* ctx){
    if(ARCH_CONTEXT_SYSCALL_SELECTOR(ctx) >= SYSCALL_COUNT){
        ARCH_CONTEXT_RETURN(ctx) = EINVAL;
        return;
    }

    handlers[ARCH_CONTEXT_SYSCALL_SELECTOR(ctx)](ctx);

    return;
}
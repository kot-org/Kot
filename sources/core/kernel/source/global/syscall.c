#include <lib/log.h>
#include <impl/arch.h>

void syscall_handler(cpu_context_t* ctx){
    log_print("syscall\n");
}
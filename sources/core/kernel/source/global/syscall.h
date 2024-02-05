#ifndef _GLOBAL_SYSCALL_H
#define _GLOBAL_SYSCALL_H 1

#include <impl/arch.h>

typedef void (*syscall_handler_t)(cpu_context_t*);

void syscall_handler(cpu_context_t* ctx);

#endif // _GLOBAL_SYSCALL_H
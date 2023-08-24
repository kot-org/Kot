#ifndef _GLOBAL_SYSCALL_H
#define _GLOBAL_SYSCALL_H 1

#include <impl/arch.h>

void syscall_handler(cpu_context_t* ctx);

#endif // _GLOBAL_SYSCALL_H
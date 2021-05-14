#include "stdlib.h"
#include "proc/syscall.h"

_Noreturn void _syscall(uint64_t syscall, uint64_t a1, uint64_t a2, uint64_t a3);

void exit(int status) {
    _syscall(SYSCALL_EXIT, status, 0, 0);
}
#include <cstddef>
#if __WORDSIZE == 64
typedef unsigned long int           uint64_t;
#else
typedef unsigned long long int      uint64_t;
#endif

#include <cstddef>
#if __WORDSIZE == 64
typedef unsigned long int           uint64_t;
#else
typedef unsigned long long int      uint64_t;
#endif

uint64_t DoSyscall(uint64_t syscall, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5){
    asm("movq %0, %%rdi" :: "r" (arg0));
    asm("movq %0, %%rsi" :: "r" (arg1));
    asm("movq %0, %%rdx" :: "r" (arg2));
    asm("movq %0, %%r10" :: "r" (arg3));
    asm("movq %0, %%r8" :: "r" (arg4));
    asm("movq %0, %%r9" :: "r" (arg5));
    asm("movq %0, %%rax" :: "r" (syscall));
    asm("int $0x80");
}

void main(int test){    
    char* file = "Alpha:/system/apps/main.elf";
    char* type = "r";
    char* msg = "I am main.elf";
    DoSyscall(2, (uint64_t)(void*)file, (uint64_t)(void*)type, 0, 0, 0, 0);
    DoSyscall(0xff, 0, 0, (uint64_t)(void*)msg, 0, 0, 0);

    while(true);
}
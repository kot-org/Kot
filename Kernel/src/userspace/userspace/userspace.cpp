#include "userspace.h"

uint64_t user_stack[1024];

__attribute__((noinline, section(".IdleTask"))) void IdleTask(){
    //enabled scheduler
    asm(".att_syntax prefix");
    asm(".intel_syntax noprefix");
    asm("movq rax, 255"); //syscall number
    asm("movq rdi, 255"); //argument 0
    asm("movq rsi, 255"); //argument 1
    asm("movq rdx, 255"); //argument 2
    asm("movq r10, 255"); //argument 3
    asm("movq r8, 255");  //argument 4
    asm("movq r9, 255");  //argument 5    
    asm(".att_syntax prefix");  
    asm("syscall"); 
    while(true);
}

extern "C" void task1(){
    asm(".att_syntax prefix");
    asm(".intel_syntax noprefix");
    asm("movq rax, 0"); //syscall number
    asm("movq rdi, 0"); //argument 0
    asm("movq rsi, 0"); //argument 1
    asm("movq rdx, 1"); //argument 2
    asm("movq r10, 0"); //argument 3
    asm("movq r8, 255");  //argument 4
    asm("movq r9, 0");  //argument 5

    asm(".att_syntax prefix");  

    char* test = "task";
    asm ("movq %0, %%r9" :: "r" (test));
    asm("syscall"); 
    while (true){
        test = "1";
        asm ("movq %0, %%r9" :: "r" (test));
        asm("syscall"); 
    }
}

extern "C" void task2(){ 
    asm(".att_syntax prefix");
    asm(".intel_syntax noprefix");
    asm("movq rax, 0"); //syscall number
    asm("movq rdi, 255"); //argument 0
    asm("movq rsi, 0"); //argument 1
    asm("movq rdx, 0"); //argument 2
    asm("movq r10, 0"); //argument 3
    asm("movq r8, 0");  //argument 4
    asm("movq r9, 0");  //argument 5    
    asm(".att_syntax prefix");  

    for(uint64_t i = 0;; i++){
        char* test = "2"; 
        asm ("movq %0, %%r9" :: "r" (test));
        asm("syscall");  
    };
}

extern "C" void task3(){
    asm(".att_syntax prefix");
    asm(".intel_syntax noprefix");
    asm("movq rax, 0"); //syscall number
    asm("movq rdi, 0"); //argument 0
    asm("movq rsi, 0"); //argument 1
    asm("movq rdx, 1"); //argument 2
    asm("movq r10, 0"); //argument 3
    asm("movq r8, 255");  //argument 4
    asm("movq r9, 0");  //argument 5

    asm(".att_syntax prefix");  

    char* test = "3";
    while (true){
        test = "3";
        asm ("movq %0, %%r9" :: "r" (test));
        asm("syscall"); 
    }
}

extern "C" void task4(){ 
    asm(".att_syntax prefix");
    asm(".intel_syntax noprefix");
    asm("movq rax, 0"); //syscall number
    asm("movq rdi, 0"); //argument 0
    asm("movq rsi, 0"); //argument 1
    asm("movq rdx, 1"); //argument 2
    asm("movq r10, 0"); //argument 3
    asm("movq r8, 0");  //argument 4
    asm("movq r9, 0");  //argument 5

    asm(".att_syntax prefix");  

    char* test = "4";
    while (true){
        test = "4";
        asm ("movq %0, %%r9" :: "r" (test));
        asm("syscall"); 
    }
}
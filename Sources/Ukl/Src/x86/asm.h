#pragma once
#include <kot/types.h>

static inline uint64_t ASMReadCr0(){                                            
    uint64_t value = 0;                            
    asm volatile("mov %%cr0, %0" : "=r"(value));                   
    return value;                                  
}                                                  
                                                   
static inline void ASMWriteCr0(uint64_t value){                                                  
    asm volatile("mov %0, %%cr0" ::"r"(value));  
}

static inline uint64_t ASMReadCr3(){                                            
    uint64_t value = 0;                            
    asm volatile("mov %%cr3, %0" : "=r"(value));                     
    return value;                                  
}                                                  
                                                   
static inline void ASMWriteCr3(uint64_t value){                                                  
    asm volatile("mov %0, %%cr3" ::"r"(value));  
}

static inline uint64_t ASMReadCr4(){                                            
    uint64_t value = 0;                            
    asm volatile("mov %%cr4, %0" : "=r"(value));                     
    return value;                                  
}                                                  
                                                   
static inline void ASMWriteCr4(uint64_t value){                                                  
    asm volatile("mov %0, %%cr4" ::"r"(value));  
}

static inline void ASMFninit(){                                                  
    asm volatile("fninit");  
}
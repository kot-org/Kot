#if defined(__x86_64__)
    #include <arch/x86-64.h>
#endif

struct KernelInfo* arch_initialize(void* boot);
#if defined(__x86_64__)
    #include <arch/x86-64.h>
#endif

struct ArchInfo_t* arch_initialize(uintptr_t boot);
void StopAllCPU();
void SetupRegistersForTask(struct thread_t* self);
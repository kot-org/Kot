#if defined(__x86_64__)
    #include <arch/x86-64.h>
#endif

struct ArchInfo_t* arch_initialize(uintptr_t boot);
KResult SendDataToStartService(struct ArchInfo_t* ArchInfo, struct thread_t* Thread, struct parameters_t* Parameters);
void StopAllCPU();
void SetupRegistersForTask(struct thread_t* self);
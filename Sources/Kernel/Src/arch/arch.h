#if defined(__x86_64__)
    #include <arch/x86-64.h>
#endif

struct ArchInfo_t* arch_initialize(uintptr_t boot);
KResult GetDataToStartService(struct ArchInfo_t* ArchInfo, struct kthread_t* thread, struct arguments_t* Parameters, uintptr_t* Data, size_t* Size);
void StopAllCPU();
void SetupRegistersForTask(struct kthread_t* self);
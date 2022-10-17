#pragma once
#include <kot/types.h>

typedef struct locker_t{
    uint64_t Mutex;
    struct kthread_t* Current;
};

bool Lock(struct locker_t* locker);
bool Release(struct locker_t* locker);
void Aquire(struct locker_t* locker);

extern "C" bool atomicLockAsm(uint64_t* mutex, uint64_t bit);
extern "C" bool atomicUnlockAsm(uint64_t* mutex, uint64_t bit);
extern "C" void atomicAcquireAsm(uint64_t* mutex, uint64_t bit);

extern "C" void JumpToKernelTaskAsm(struct ContextStack* context);
extern "C" void SaveTaskAsm();
extern "C" void RestoreTaskAsm();
extern "C" void GetTaskAsm(struct threadInfo_t* ThreadInfo);
#pragma once

#include <kot/sys.h>
#include <kot/types.h>
#include <kot/atomic.h>
#include <kot/keyhole.h>
#include <kot/modules/ipc/ipc.h>

kthread_t IPCInitialize();

void IPCHandler(uint64_t IPCTask, uint64_t GlobalPurpose, uint64_t EncodedName);

struct IPCBlock_t {
    kthread_t Thread;
    uint64_t EncodedName;
    struct IPCBlock_t* Next;
};

struct IPCSaver_t {
    struct IPCBlock_t* Main;
    struct IPCBlock_t* Last;
    uint64_t NumberOfTask;
};
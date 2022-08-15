#pragma once

#include <kot/sys.h>
#include <kot/types.h>
#include <kot/atomic.h>
#include <kot/keyhole.h>
#include <kot/modules/ipc/ipc.h>

thread IPCInitialize();

void IPCHandler(uint64_t IPCTask, uint64_t GlobalPurpose, uint64_t EncodedName);

struct IPCBlock_t {
    thread thread;
    uint64_t EncodedName;
    struct IPCBlock_t* Next;
};

struct IPCSaver_t {
    struct IPCBlock_t* Main;
    struct IPCBlock_t* Last;
    uint64_t NumberOfTask;
};
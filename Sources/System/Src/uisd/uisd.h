#pragma once

#include <kot/sys.h>
#include <kot/types.h>
#include "../core/main.h"
#include <kot/atomic.h>
#include <kot/keyhole.h>
#include <kot/utils/vector.h>
#include <kot/uisd.h>
#include <kot++/stack.h>

thread_t UISDInitialize(process_t* process);

KResult UISDCallbackStatu(uint64_t IPCTask, thread_t Callback, uint64_t Callbackarg, KResult Status);
void UISDHandler(uint64_t IPCTask, enum ControllerTypeEnum Controller, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

struct controller_info_t {
    bool IsLoad;
    uintptr_t Data;
    ksmem_t DataKey;
    std::Stack* WaitingTasks;
    uint64_t NumberOfWaitingTasks;
};

struct callbackget_info_t{
    enum ControllerTypeEnum Controller;
    process_t Self;
    uintptr_t Address;
    thread_t Callback; 
    uint64_t Callbackarg;
};
#pragma once

#include <kot/sys.h>
#include <kot/types.h>
#include "../core/main.h"
#include <kot/atomic.h>
#include <kot/keyhole.h>
#include <kot/utils/vector.h>
#include <kot/uisd.h>
#include <kot++/stack.h>

kot_thread_t UISDInitialize(kot_process_t* process);

KResult UISDCallbackStatu(uint64_t IPCTask, kot_thread_t Callback, uint64_t Callbackarg, KResult Status);
void UISDHandler(uint64_t IPCTask, enum kot_uisd_controller_type_enum Controller, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

struct controller_info_t {
    bool IsLoad;
    void* Data;
    kot_key_mem_t DataKey;
    std::Stack* WaitingTasks;
    uint64_t NumberOfWaitingTasks;
};

struct callbackget_info_t{
    enum kot_uisd_controller_type_enum Controller;
    kot_process_t Self;
    void* Address;
    kot_thread_t Callback; 
    uint64_t Callbackarg;
};
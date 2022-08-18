#pragma once

#include <kot/sys.h>
#include <kot/types.h>
#include <core/main.h>
#include <kot/atomic.h>
#include <kot/keyhole.h>
#include <kot/utils/vector.h>
#include <kot/modules/uisd/uisd.h>

thread UISDInitialize();

void UISDHandler(uint64_t IPCTask, enum ControllerTypeEnum Controller, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

struct controller_info_t {
    bool IsLoad;
    uintptr_t Data;
    ksmem_t DataKey;
    vector_t* WaitingTask;
};

struct callback_info_t{
    enum ControllerTypeEnum Controller;
    process_t Self;
    uintptr_t Address;
    thread Callback; 
    uint64_t Callbackarg;
};
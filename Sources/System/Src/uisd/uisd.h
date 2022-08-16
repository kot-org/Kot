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
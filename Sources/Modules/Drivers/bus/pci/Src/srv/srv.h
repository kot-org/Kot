#pragma once
#include <core/main.h>

void InitSrv();

KResult GetBARNum(thread_t Callback, uint64_t CallbackArg, PCIDeviceID_t device);
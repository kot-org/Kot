#pragma once 

#include <core/main.h>
#include <port/port.h>
#include <kot/types.h>

void SrvAddDevice(class Device* Device);
void SrvRemoveDevice(Device* Device);

void SrvReadWriteHandler(thread_t Callback, uint64_t CallbackArg, Device* Device, uint64_t Start, size64_t Size, bool IsWrite);
KResult SrvRead(Device* Device, uint64_t Start, size64_t Size);
KResult SrvWrite(Device* Device, uint64_t Start, size64_t Size);
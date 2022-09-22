#pragma once 

#include <core/main.h>
#include <port/port.h>
#include <kot/types.h>

extern class Device** Devices;
extern uint64_t DevicesIndex;

void SrvAddDevice(class Device* Device);
void SrvRemoveDevice(Device* Device);

void SrvReadWriteHandler(thread_t Callback, uint64_t CallbackArg, uint64_t Index, uint64_t Start, size64_t Size, bool IsWrite);
KResult SrvRead(uint64_t Index, uint64_t Start, size64_t Size);
KResult SrvWrite(uint64_t Index, uint64_t Start, size64_t Size);
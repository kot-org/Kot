#pragma once

extern "C" uint64_t DoSyscall(uint64_t syscall, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5);

//IPC

struct Parameters{
    uint64_t Parameter0;
    uint64_t Parameter1;
    uint64_t Parameter2;
    uint64_t Parameter3;
    uint64_t Parameter4;
    uint64_t Parameter5;
}__attribute__((packed));

struct DeviceTaskAdressStruct{
	uint8_t type:3;
    uint16_t L1:10;
    uint16_t L2:10;
    uint16_t L3:10;
    uint16_t FunctionID:9;
}__attribute__((packed));
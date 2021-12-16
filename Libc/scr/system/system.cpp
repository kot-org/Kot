#include "system.h"

void printf(const char *str, ...){
    DeviceTaskAdressStruct device;
    Parameters parameters;
    device.type = 0;
    device.L1 = 0;
    device.L2 = 0;
    device.L3 = 0;
    device.FunctionID = 0;  
    parameters.Parameter0 = 3;
    parameters.Parameter1 = (uint64_t)(void*)str;
    
    DoSyscall(0x03, (uint64_t)(void*)&device, (uint64_t)(void*)&parameters, 0, 0, 0, 0);
}
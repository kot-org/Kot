#include <cstddef>
#include <stdint.h>

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

uint64_t DoSyscall(uint64_t syscall, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5){
    asm("movq %0, %%rdi" :: "r" (arg0));
    asm("movq %0, %%rsi" :: "r" (arg1));
    asm("movq %0, %%rdx" :: "r" (arg2));
    asm("movq %0, %%r10" :: "r" (arg3));
    asm("movq %0, %%r8" :: "r" (arg4));
    asm("movq %0, %%r9" :: "r" (arg5));
    asm("movq %0, %%rax" :: "r" (syscall));
    asm("int $0x80");

    register uint64_t rdi asm("rdi");
    return rdi;
}

void IPCFunctionTest(uint64_t PID){
    char* msg = "Hello from IPC";
    DoSyscall(0xff, 0, 0, (uint64_t)(void*)msg, 0, 0, 0);
    uint64_t memoryAdd = DoSyscall(0x0C, 0x1000, 0x100000, 0, 0, 0, 0);
    *(uint8_t*)((uint64_t)0x100000 + 0x100) = 0xff;
    //exit
    DoSyscall(0x3C, 0, memoryAdd, 0, 0, 0, 0);
}

void main(uint64_t test){    
    char* file = "Alpha:/system/apps/main.elf";
    char* type = "r";
    char* msg = "I am main.elf";
    DoSyscall(2, (uint64_t)(void*)file, (uint64_t)(void*)type, 0, 0, 0, 0);
    if(test == 0xff){
        DoSyscall(0xff, 0, 3, (uint64_t)(void*)msg, 0, 0, 0);
    }
    DoSyscall(0xff, 0, 0, (uint64_t)(void*)msg, 0, 0, 0);

    //trying IPC
    DeviceTaskAdressStruct device;
    device.type = 3;
    device.L1 = 3;
    device.L2 = 3;
    device.L3 = 3;
    device.FunctionID = 0;
    DoSyscall(0x16, (uint64_t)(void*)IPCFunctionTest, (uint64_t)(void*)&device, 0, 0, 0, 0);
    Parameters parameters;
    uint64_t memoryAdd = DoSyscall(0x17, (uint64_t)(void*)&device, (uint64_t)(void*)&parameters, 0, 0, 0, 0);
    //let's creat share memory
    DoSyscall(0x0D, memoryAdd, 0x20000, 0, 0, 0, 0);
    if(*(uint8_t*)((uint64_t)0x20000 + 0x100) == 0xff){
        char* sucess = "Sucess";
        DoSyscall(0xff, 0, 0, (uint64_t)(void*)sucess, 0, 0, 0);
    }else{
        char* error = "Error";
        DoSyscall(0xff, 0, 0, (uint64_t)(void*)error, 0, 0, 0);
    }
    //exit
    DoSyscall(0x3C, 0, 0, 0, 0, 0, 0);


    while(true);
}


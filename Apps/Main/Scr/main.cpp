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

// uint64_t DoSyscall(uint64_t syscall, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5){
//     asm("movq %0, %%rdi" :: "r" (arg0));
//     asm("movq %0, %%rsi" :: "r" (arg1));
//     asm("movq %0, %%rdx" :: "r" (arg2));
//     asm("movq %0, %%r10" :: "r" (arg3));
//     asm("movq %0, %%r8" :: "r" (arg4));
//     asm("movq %0, %%r9" :: "r" (arg5));
//     asm("movq %0, %%rax" :: "r" (syscall));
//     asm("int $0x80");

//     register uint64_t rdi asm("rdi");
//     return rdi;
// }

extern "C" uint64_t DoSyscall(uint64_t syscall, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5);

void IPCFunctionTest(uint64_t PID){
    char* msg = "Hello from IPC";
    DoSyscall(0xff, 0, 0, (uint64_t)(void*)msg, 0, 0, 0);
    uint64_t memoryAdd = 0;
    uint64_t vmadd = 0x100000;
    DoSyscall(0x0C, 0x1000, (uint64_t)&vmadd, (uint64_t)&memoryAdd, (uint64_t)false, 0, 0);
    *(uint8_t*)((uint64_t)0x100000 + 0x100) = 0xff;
    //exit
    DoSyscall(0x3C, 0, memoryAdd, 0, 0, 0, 0);
}

#define MaxPath 512
#define MaxName 256
#define MaxPassword 256
#define MaxUserRight 256   

struct GUID{  
    uint32_t Data1;  
    uint16_t Data2;  
    uint16_t Data3;  
    uint64_t Data4;
}__attribute__((packed));

struct Time{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t days;
    uint8_t months;
    uint64_t years;
}__attribute__((packed));

struct timeInfoFS{
    Time CreateTime;
    Time ModifyTime;
}__attribute__((packed));

struct FileInfo{
    /* location info */
    uint64_t LastClusterOfTheFile;
    uint64_t ClusterHeaderPostition;
    size_t BytesSize;  
    size_t ClusterSize; //number of Cluster 
    char Path[MaxPath];
    char Name[MaxName];
    /* userRight */
    char Password[MaxPassword];
    GUID Owner;
    /* time */
    timeInfoFS TimeInfoFS;
    uint64_t NextCluster;
}__attribute__((packed));

struct File{
        FileInfo fileInfo;
        char* Mode;            
        void* reverved;
}__attribute__((packed));

void main(uint64_t test){    
    char* msg = "I am main.elf";
    char* msgk = "kernel";
    DoSyscall(0xff, 0, 0, (uint64_t)(void*)msg, 0, 0, 0);
    // char* file = "Alpha:/system/apps/main.elf";
    // char* type = "r";
    // File filereturn;
    //DoSyscall(2, (uint64_t)(void*)file, (uint64_t)(void*)type, (uint64_t)&filereturn, 0, 0, 0);
    if(test == 0xff){
        DoSyscall(0xff, 0, 3, (uint64_t)(void*)msgk, 0, 0, 0);
    }

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
    //IPC kernel
    device.type = 0;
    device.L1 = 0;
    device.L2 = 0;
    device.L3 = 0;
    device.FunctionID = 0;  
    parameters.Parameter0 = 1;
    parameters.Parameter1 = (uint64_t)(void*)msgk;
    DoSyscall(0x17, (uint64_t)(void*)&device, (uint64_t)(void*)&parameters, 0, 0, 0, 0);
    //let's creat share memory
    uint64_t vmadd = 0x20000;
    DoSyscall(0x0D, memoryAdd, (uint64_t)&vmadd, 0, 0, 0, 0);
    if(*(uint8_t*)((uint64_t)0x20000 + 0x100) == 0xff){
        *(uint8_t*)((uint64_t)0x20000 + 0x100) = 0x50;
        char* sucess = "Sucess";
        DoSyscall(0xff, 0, 0, (uint64_t)(void*)sucess, 0, 0, 0);
    }else{
        char* error = "Error";
        DoSyscall(0xff, 0, 0, (uint64_t)(void*)error, 0, 0, 0);
    }
    //exit
    DoSyscall(0x3C, 0, 0, 0, 0, 0, 0);
}


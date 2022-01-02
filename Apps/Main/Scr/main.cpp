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
    uint16_t FunctionID:10;
}__attribute__((packed));

extern "C" uint64_t DoSyscall(uint64_t syscall, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5);

void IPCFunctionTest(uint64_t PID){
    uint64_t memoryAdd = 0;
    uint64_t vmadd = 0x100000;
    DoSyscall(0x0, 0x1000, (uint64_t)&vmadd, (uint64_t)&memoryAdd, (uint64_t)false, 0, 0);
    *(uint8_t*)((uint64_t)0x100000 + 0x100) = 0xff;
    //exit
    DoSyscall(0x5, 0, memoryAdd, 0, 0, 0, 0);
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

void IRQTest();
void main(uint64_t test){    
    char* msg1 = "I am main.elf";

    //trying IPC
    DeviceTaskAdressStruct device;
    device.type = 3;
    device.L1 = 3;
    device.L2 = 3;
    device.L3 = 3;
    device.FunctionID = 0;
    DoSyscall(0x3, (uint64_t)(void*)IPCFunctionTest, (uint64_t)(void*)&device, 0, 0, 0, 0);
    Parameters parameters;
    uint64_t memoryAdd = DoSyscall(0x4, (uint64_t)(void*)&device, (uint64_t)(void*)&parameters, 0, 0, 0, 0);
    //IPC kernel
    device.type = 0;
    device.L1 = 0;
    device.L2 = 0;
    device.L3 = 0;
    device.FunctionID = 0;  
    parameters.Parameter0 = 3;
    parameters.Parameter1 = (uint64_t)(void*)msg1;
    DoSyscall(0x4, (uint64_t)(void*)&device, (uint64_t)(void*)&parameters, 0, 0, 0, 0);
    DoSyscall(0x4, (uint64_t)(void*)&device, (uint64_t)(void*)&parameters, 0, 0, 0, 0);
    DoSyscall(0x4, (uint64_t)(void*)&device, (uint64_t)(void*)&parameters, 0, 0, 0, 0);
    DoSyscall(0x4, (uint64_t)(void*)&device, (uint64_t)(void*)&parameters, 0, 0, 0, 0);
    DoSyscall(0x4, (uint64_t)(void*)&device, (uint64_t)(void*)&parameters, 0, 0, 0, 0);
    DoSyscall(0x4, (uint64_t)(void*)&device, (uint64_t)(void*)&parameters, 0, 0, 0, 0);
    DoSyscall(0x4, (uint64_t)(void*)&device, (uint64_t)(void*)&parameters, 0, 0, 0, 0);
    //let's creat share memory
    uint64_t vmadd = 0x20000;
    DoSyscall(0x01, memoryAdd, (uint64_t)&vmadd, 0, 0, 0, 0);
    if(*(uint8_t*)((uint64_t)0x20000 + 0x100) == 0xff){
        *(uint8_t*)((uint64_t)0x20000 + 0x100) = 0x50;
        char* sucess = "Sucess";
        parameters.Parameter1 = (uint64_t)(void*)sucess;
        DoSyscall(0x4, (uint64_t)(void*)&device, (uint64_t)(void*)&parameters, 0, 0, 0, 0);
    }else{
        char* error = "Error";
        parameters.Parameter1 = (uint64_t)(void*)error;
        DoSyscall(0x4, (uint64_t)(void*)&device, (uint64_t)(void*)&parameters, 0, 0, 0, 0);
    }
    DoSyscall(0x9, (uint64_t)(void*)IRQTest, 0x1, 0, 0, 0, 0);
    
    //exit
    DoSyscall(0x6, 0, 0, 0, 0, 0, 0);
}

const char ASCIITable[] = {
         0 ,  0 , '1', '2',
        '3', '4', '5', '6',
        '7', '8', '9', '0',
        '-', '=',  0 ,  0 ,
        'a', 'z', 'e', 'r',
        't', 'y', 'u', 'i',
        'o', 'p', '[', ']',
         0 ,  0 , 'q', 's',
        'd', 'f', 'g', 'h',
        'j', 'k', 'l', 'm',
        '\'','`',  0 , '\\',
        'w', 'x', 'c', 'v',
        'b', 'n', ';', ',',
        '.', '/',  0 , '*',
         0 , ' '
    };
void IRQTest(){
    DeviceTaskAdressStruct device;
    Parameters parameters;
    device.type = 0;
    device.L1 = 0;
    device.L2 = 0;
    device.L3 = 0;
    device.FunctionID = 0;  
    parameters.Parameter0 = 3;
    uint8_t scancode = DoSyscall(0x12, 0x8, 0x60, 0, 0, 0, 0);
    if (scancode > 58) DoSyscall(0xA, 0, 0, 0, 0, 0, 0);
    char* msg;
    msg[0] = ASCIITable[scancode];
    msg[1] = 0;
    parameters.Parameter1 = (uint64_t)(void*)msg;
    DoSyscall(0x4, (uint64_t)(void*)&device, (uint64_t)(void*)&parameters, 0, 0, 0, 0);
    DoSyscall(0xA, 0, 0, 0, 0, 0, 0);
}
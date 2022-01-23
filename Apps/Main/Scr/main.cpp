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

extern "C" uint64_t DoSyscall(uint64_t syscall, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5);

void IPCFunctionTest(){
    uint64_t memoryAdd = 0;
    uint64_t vmadd = 0x100000;
    DoSyscall(0x0, 0x1000, (uint64_t)&vmadd, (uint64_t)&memoryAdd, (uint64_t)false, 0, 0);
    *(uint8_t*)((uint64_t)0x100000 + 0x100) = 0xff;
    //exit
    DoSyscall(0x6, 0, memoryAdd, 0, 0, 0, 0);
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
void IRQTest(){
    DoSyscall(0x12, 8, 0x3f8, 'o', 0, 0, 0);
    DoSyscall(0x12, 8, 0x3f8, 'k', 0, 0, 0);
    DoSyscall(0x6, 0, 0, 0, 0, 0, 0);
}
void Thread1(){
    DoSyscall(0x12, 3, 0, 0, 0, 0, 0);
    while(true){
        __asm__ volatile("outb %b0, %w1" : : "a" ('1'), "Nd" (0x3f8));
    }    
}
void Thread2(){
    DoSyscall(0x12, 3, 0, 0, 0, 0, 0);
    while(true){
        __asm__ volatile("outb %b0, %w1" : : "a" ('2'), "Nd" (0x3f8));
    }    
}
void main(){        
    DoSyscall(0xB, (uint64_t)(void*)IRQTest, 0x1, 0, 0, 0, 0);
    DoSyscall(0x12, 3, 0, 0, 0, 0, 0);
    __asm__ volatile("outb %b0, %w1" : : "a" ('o'), "Nd" (0x3f8));
    __asm__ volatile("outb %b0, %w1" : : "a" ('u'), "Nd" (0x3f8));
    __asm__ volatile("outb %b0, %w1" : : "a" ('t'), "Nd" (0x3f8));

    uint64_t test = DoSyscall(0xf, (uint64_t)(void*)Thread1, 0, 0, 0, 0, 0);
    uint64_t test1 = DoSyscall(0xf, (uint64_t)(void*)Thread2, 0, 0, 0, 0, 0);
    DoSyscall(0x10, test1, 0, 0, 0, 0, 0);
    DoSyscall(0x10, test, 0, 0, 0, 0, 0);
    //exit
    DoSyscall(0x7, 0, 0, 0, 0, 0, 0);
}

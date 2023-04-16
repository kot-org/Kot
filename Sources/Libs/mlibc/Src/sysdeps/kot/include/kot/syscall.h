#ifndef KOT_SYSCALL_H
#define KOT_SYSCALL_H

#include <stdint.h>

#define Syscall_Count 0x17 + 1

#define KSys_CreateMemoryField 0x0
#define KSys_AcceptMemoryField 0x1
#define KSys_CloseMemoryField 0x2
#define KSys_GetTypeMemoryField 0x3

#define KSys_CreateProc 0x4
#define KSys_CloseProc 0x5

#define KSys_Close 0x6
#define KSys_Exit 0x7
#define KSys_Pause 0x8
#define KSys_UnPause 0x9

#define KSys_Map 0xA
#define KSys_Unmap 0xB
#define KSys_GetPhysical 0xC

#define KSys_Event_Create 0xD
#define KSys_Event_Bind 0xE
#define KSys_Event_Unbind 0xF
#define KSys_Event_Trigger 0x10
#define KSys_Event_Close 0x11

#define KSys_CreateThread 0x12
#define KSys_DuplicateThread 0x13
#define KSys_ExecThread 0x14

#define KSys_Keyhole_CloneModify 0x15
#define KSys_Keyhole_Verify 0x16

#define KSys_Logs 0x17

#define KFAIL               0
#define KSUCCESS            1
#define KBUSY               2
#define KNOTALLOW           3
#define KKEYVIOLATION       4
#define KMEMORYVIOLATION    5

typedef uint64_t kot_key_t; 
typedef kot_key_t kot_process_t;
typedef kot_key_t kot_thread_t;
typedef kot_key_t kot_event_t;
typedef kot_key_t kot_ksmem_t;

struct KotSpecificData_t{
    /* Memory */
    uint64_t MMapPageSize;
    /* Heap */
    uint64_t HeapLocation;
    /* UISD */
    kot_thread_t UISDHandler;
    kot_process_t UISDHandlerProcess;
    /* FreeMemorySpace */
    uintptr_t FreeMemorySpace;
    /* VFS */
    kot_thread_t VFSHandler;
}__attribute__((aligned(0x1000)));

extern struct KotSpecificData_t KotSpecificData;

__attribute__((always_inline))
static inline uint64_t Syscall_0(uint64_t call) {
    volatile uint64_t ret;
    asm volatile("syscall" : "=a"(ret) : "a"(call)); 
    return ret;
}

__attribute__((always_inline))
static uint64_t Syscall_8(uint64_t call, uint64_t arg0) {
    volatile uint64_t ret;
    asm volatile("syscall" : "=a"(ret) : "a"(call), "D"(arg0) : "memory"); 
    return ret;
}

__attribute__((always_inline))
static uint64_t Syscall_16(uint64_t call, uint64_t arg0, uint64_t arg1) {
    volatile uint64_t ret;
    asm volatile("syscall" : "=a"(ret) : "a"(call), "D"(arg0), "S"(arg1) : "memory"); 
    return ret;
}

__attribute__((always_inline))
static uint64_t Syscall_24(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2) {
    volatile uint64_t ret;
    asm volatile("syscall" : "=a"(ret) : "a"(call), "D"(arg0), "S"(arg1), "d"(arg2) : "memory"); 
    return ret;
}

__attribute__((always_inline))
static uint64_t Syscall_32(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    volatile uint64_t ret;
	register uint64_t arg3r asm("r10") = arg3; // put arg3 in r10
    asm volatile("syscall" : "=a"(ret) : "a"(call), "D"(arg0), "S"(arg1), "d"(arg2), "r"(arg3r) : "memory"); 
    return ret;
}

__attribute__((always_inline))
static uint64_t Syscall_40(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4) {
    volatile uint64_t ret;
	register uint64_t arg3r asm("r10") = arg3; // put arg3 in r10
	register uint64_t arg4r asm("r9") = arg4; // put arg4 in r9
    asm volatile("syscall" : "=a"(ret) : "a"(call), "D"(arg0), "S"(arg1), "d"(arg2), "r"(arg3r), "r"(arg4r) : "memory"); 
    return ret;
}

__attribute__((always_inline))
static uint64_t Syscall_48(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
    volatile uint64_t ret;
	register uint64_t arg3r asm("r10") = arg3; // put arg3 in r10
	register uint64_t arg4r asm("r9") = arg4; // put arg4 in r9
	register uint64_t arg5r asm("r8") = arg5; // put arg5 in r8
    asm volatile("syscall" : "=a"(ret) : "a"(call), "D"(arg0), "S"(arg1), "d"(arg2), "r"(arg3r), "r"(arg4r), "r"(arg5r) : "memory"); 
    return ret;
}

#endif

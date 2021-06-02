#include "system.h"
#define PIC1_DATA 0x21
#define PIC2_DATA 0xA1
bool IsIntInit = false;
bool test = true;

extern "C" uint64_t SyscallEntry(int arg0, int arg1, int arg2, int reserved, int arg4, int arg5){
    register uint64_t syscall asm("r14");
    register uint64_t arg3 asm("r15");

    switch(syscall){
        case 0x01:
            break;
    }
    //printf("%s", arg5);
    //globalGraphics->Update(); 
   
    return 0;
}

extern "C" uint64_t SystemExit(uint64_t ErrorCode){

}


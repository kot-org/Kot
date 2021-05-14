#include "msr.h"

namespace msr{
    void ReadMSR(uint32_t msr,uint32_t* data,uint32_t* dataUpper) {
        asm volatile("rdmsr" : "=a"(*data), "=d"(*dataUpper) : "c"(msr));
    }

    uint32_t GetCPUTemp(){
        uint32_t Temp;
        uint32_t TempUpper;

        ReadMSR(MSR_CPU_TEMP,&Temp,&TempUpper);
        return ((Temp >> 16) & 0x7f) - 30;
    }    
}

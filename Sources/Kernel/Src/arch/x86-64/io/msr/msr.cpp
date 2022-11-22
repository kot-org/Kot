#include <arch/x86-64/io/msr/msr.h>

namespace msr{
    void ReadMSR(uint32_t msr,uint32_t* data,uint32_t* dataUpper) {
        asm volatile("rdmsr" : "=a"(*data), "=d"(*dataUpper) : "c"(msr));
    }

    uint64_t rdmsr(uint32_t msr){
        uint32_t lower;
        uint32_t upper;
        asm volatile("rdmsr" : "=a"(lower), "=d"(upper) : "c"(msr));
        return ((uint64_t) upper << 32) | lower;
    }
    
    void wrmsr(uint32_t msr, uint64_t value){
        uint32_t lower = (uint32_t) value;
        uint32_t upper = (uint32_t) (value >> 32);
        asm volatile("wrmsr" :: "a"(lower), "c"(msr), "d"(upper));
    }

    uint32_t GetCPUTemp(){
        uint32_t Temp;
        uint32_t TempUpper;

        ReadMSR(MSR_CPU_TEMP, &Temp, &TempUpper);
        return ((Temp >> 16) & 0x7f) - 30;
    }    
}

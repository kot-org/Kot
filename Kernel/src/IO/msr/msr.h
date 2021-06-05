#pragma once
#include <stdint.h>

namespace msr{
    #define MSR_CPU_TEMP 0x1A2

    void ReadMSR(uint32_t msr,uint32_t* low,uint32_t* high);
    uint64_t rdmsr(uint32_t msr);
    void wrmsr(uint32_t msr, uint64_t value);
    
    uint32_t GetCPUTemp();    
}

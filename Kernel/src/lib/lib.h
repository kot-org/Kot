#pragma once

#include <stdint.h>

struct GUID{  
    uint32_t Data1;  
    uint16_t Data2;  
    uint16_t Data3;  
    uint64_t Data4;
}__attribute__((packed));
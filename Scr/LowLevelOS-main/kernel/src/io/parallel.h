#pragma once

#include <stdint.h>
#include "ports.h"

#define LPT1_DATA 0x378
#define LPT1_STATUS 0x379
#define LPT1_CONTROL 0x37A

class Parallel
{
private:
    void delay();
    void waitPrinterReady();
    void tellPrinterRead();
public:
    void writeChar(unsigned char ch);
    void writeChars(const char* ch);
    uint8_t getStatusFlag(int flag);
    uint8_t getControlFlag(int flag);
};
#pragma once
#include "../../lib/types.h"
#include "../../arch/x86-64/io/io.h"

class RealTimeClock {
    private:
        unsigned char getRegister(int reg);
        int getUpdateInProgress();
        void waitUpdate();
    public:
        void waitSeconds(uint32_t secs);
        uint32_t readHours();
        uint32_t readMinutes();
        uint32_t readSeconds();
        uint32_t readTime();
        uint32_t readDay();
        uint32_t readMonth();
        uint32_t readYear();
};
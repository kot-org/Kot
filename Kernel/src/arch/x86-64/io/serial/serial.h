#pragma once
#include "../io.h"
#include "../../../../logs/logs.h"
#include "../../../../lib/stdio.h"
#define COM1 0x3f8


class SerialPort{
public:
    void Initialize();
    void Write(char chr);
    void Print(const char* chr);
    void Printf(const char* str, ...);
    void ClearMonitor();
};

extern SerialPort* globalCOM1;
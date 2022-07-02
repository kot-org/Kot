#pragma once
#include <arch/x86-64/io/io.h>
#include <lib/stdio.h>
#include <lib/limits.h>

#define COM1 0x3f8
#define BOCHSLOG 0xe9

#define SerialReset (char*)"\033[1;0m"
#define SerialBLACK (char*)"\033[1;30m"
#define SerialRED (char*)"\033[1;31m"
#define SerialGREEN (char*)"\033[1;32m"
#define SerialYELLOW (char*)"\033[1;33m"
#define SerialBLUE (char*)"\033[1;34m"
#define SerialPINK (char*)"\033[1;35m"
#define SerialCYAN (char*)"\033[1;36m"
#define SerialWHITE (char*)"\033[1;37m"

namespace SerialPort{
    void Initialize();
    void Write(char chr);
    void Print(const char* chr);
    void Printf(const char* str, ...);
    void ClearMonitor();
};
#pragma once
#include <arch/x86-64/io/io.h>
#include <lib/stdio.h>
#include <lib/limits.h>

#define COM1            0x3f8
#define BOCHSLOG        0xe9

#define SERIAL_RESET    (char*)"\033[1;0m"
#define SERIAL_BLACK    (char*)"\033[1;30m"
#define SERIAL_RED      (char*)"\033[1;31m"
#define SERIAL_GREEN    (char*)"\033[1;32m"
#define SERIAL_YELLOW   (char*)"\033[1;33m"
#define SERIAL_BLUE     (char*)"\033[1;34m"
#define SERIAL_PINK     (char*)"\033[1;35m"
#define SERIAL_CYAN     (char*)"\033[1;36m"
#define SERIAL_WHITE    (char*)"\033[1;37m"

namespace SerialPort{
    void Initialize();
    void Write(char chr);
    void Print(const char* chr);
    void Print(const char* chr, uint64_t charNum);
    void Printf(const char* str, ...);
    void ClearMonitor();
};
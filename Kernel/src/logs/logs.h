#pragma once
#include "../arch/x86-64/io/serial/serial.h"

class Logs{
public:
    void Message(const char* str, ...);
    void Successful(const char* str, ...);
    void Warning(const char* str, ...);
    void Error(const char* str, ...);
};

extern Logs* globalLogs;
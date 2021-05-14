#pragma once
#include "ports.h"

class SerialPort
{
private:
    int isTransmitEmpty();
    int isReceived();
public:
    void Init();
    void Write(char chr);
    void Write(const char* chr);
    void Write(const char* chr,const char* chr2);
    void Write(const char* chr,const char* chr2,const char* chr3);
    char Read();
    void ClearMonitor();
};

extern SerialPort* GlobalCOM1;
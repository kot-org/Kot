#include "logs.h"

Logs* globalLogs;
static uint64_t mutexLog;

void Logs::Message(const char* str, ...){
    Atomic::atomicSpinlock(&mutexLog, 0);
    Atomic::atomicLock(&mutexLog, 0);
    va_list args;
    va_start(args, str);
    
    int index = 0;
    while(str[index] != 0) {
        if(str[index] == '%' && (str[index + 1] == 'd' || str[index + 1] == 'i')) {
            globalCOM1->Print(to_string(va_arg(args, int))); //int or decimal
            index++;
        } else if (str[index] == '%' && str[index+1] == 'u') {
            globalCOM1->Print(to_string(va_arg(args, uint64_t))); //uint
            index++;
        } else if (str[index] == '%' && (str[index+1] == 'x' || str[index+1] == 'X')) {
            uint64_t val = va_arg(args, uint64_t); //hex
            if(val > uint32_Limit)
                globalCOM1->Print(to_hstring(val)); 
            else if (val > uint16_Limit) 
                globalCOM1->Print(to_hstring((uint32_t)val));
            else if (val > uint8_Limit) 
                globalCOM1->Print(to_hstring((uint16_t)val));
            else
                globalCOM1->Print(to_hstring((uint8_t)val));                
            index++;    
        } else if (str[index] == '%' && (str[index + 1] == 'f' || str[index+1] == 'F')) {
            globalCOM1->Print(to_string(va_arg(args, double))); //float
            index++;
        } else if (str[index] == '%' && str[index+1] == 'c' && str[index+2] != 'o') {
            globalCOM1->Print(to_string(va_arg(args, int))); //char
            index++;
        } else if (str[index] == '%' && str[index+1] == 's') {
            globalCOM1->Print(va_arg(args, const char*)); //string
            index++;
        } else if (str[index] == '%' && str[index+1] == 'p') {
            globalCOM1->Print(to_string((uint64_t)va_arg(args, void*))); //address
            index++;
        } else {
            globalCOM1->Print(to_string(str[index])); //char
        }
        index++;
    }

    globalCOM1->Print("\n");
    va_end(args);
    Atomic::atomicUnlock(&mutexLog, 0);
}   

void Logs::Successful(const char* str, ...){
    Atomic::atomicSpinlock(&mutexLog, 0);
    Atomic::atomicLock(&mutexLog, 0);
    va_list args;
    va_start(args, str);
    
    globalCOM1->Print(SerialGREEN);
    globalCOM1->Print("[OK] ");
    globalCOM1->Print(SerialReset);
    
    int index = 0;
    while(str[index] != 0) {
        if(str[index] == '%' && (str[index + 1] == 'd' || str[index + 1] == 'i')) {
            globalCOM1->Print(to_string(va_arg(args, int))); //int or decimal
            index++;
        } else if (str[index] == '%' && str[index+1] == 'u') {
            globalCOM1->Print(to_string(va_arg(args, uint64_t))); //uint
            index++;
        } else if (str[index] == '%' && (str[index+1] == 'x' || str[index+1] == 'X')) {
            uint64_t val = va_arg(args, uint64_t); //hex
            if(val > uint32_Limit)
                globalCOM1->Print(to_hstring(val)); 
            else if (val > uint16_Limit) 
                globalCOM1->Print(to_hstring((uint32_t)val));
            else if (val > uint8_Limit) 
                globalCOM1->Print(to_hstring((uint16_t)val));
            else
                globalCOM1->Print(to_hstring((uint8_t)val));                
            index++;    
        } else if (str[index] == '%' && (str[index + 1] == 'f' || str[index+1] == 'F')) {
            globalCOM1->Print(to_string(va_arg(args, double))); //float
            index++;
        } else if (str[index] == '%' && str[index+1] == 'c' && str[index+2] != 'o') {
            globalCOM1->Print(to_string(va_arg(args, int))); //char
            index++;
        } else if (str[index] == '%' && str[index+1] == 's') {
            globalCOM1->Print(va_arg(args, const char*)); //string
            index++;
        } else if (str[index] == '%' && str[index+1] == 'p') {
            globalCOM1->Print(to_string((uint64_t)va_arg(args, void*))); //address
            index++;
        } else {
            globalCOM1->Print(to_string(str[index])); //char
        }
        index++;
    }

    globalCOM1->Print("\n");
    va_end(args);
    Atomic::atomicUnlock(&mutexLog, 0);
}

void Logs::Warning(const char* str, ...){
    Atomic::atomicSpinlock(&mutexLog, 0);
    Atomic::atomicLock(&mutexLog, 0);
    va_list args;
    va_start(args, str);
    
    globalCOM1->Print(SerialYELLOW);
    globalCOM1->Print("[Warning] ");
    globalCOM1->Print(SerialReset);
    
    int index = 0;
    while(str[index] != 0) {
        if(str[index] == '%' && (str[index + 1] == 'd' || str[index + 1] == 'i')) {
            globalCOM1->Print(to_string(va_arg(args, int))); //int or decimal
            index++;
        } else if (str[index] == '%' && str[index+1] == 'u') {
            globalCOM1->Print(to_string(va_arg(args, uint64_t))); //uint
            index++;
        } else if (str[index] == '%' && (str[index+1] == 'x' || str[index+1] == 'X')) {
            uint64_t val = va_arg(args, uint64_t); //hex
            if(val > uint32_Limit)
                globalCOM1->Print(to_hstring(val)); 
            else if (val > uint16_Limit) 
                globalCOM1->Print(to_hstring((uint32_t)val));
            else if (val > uint8_Limit) 
                globalCOM1->Print(to_hstring((uint16_t)val));
            else
                globalCOM1->Print(to_hstring((uint8_t)val));                
            index++;    
        } else if (str[index] == '%' && (str[index + 1] == 'f' || str[index+1] == 'F')) {
            globalCOM1->Print(to_string(va_arg(args, double))); //float
            index++;
        } else if (str[index] == '%' && str[index+1] == 'c' && str[index+2] != 'o') {
            globalCOM1->Print(to_string(va_arg(args, int))); //char
            index++;
        } else if (str[index] == '%' && str[index+1] == 's') {
            globalCOM1->Print(va_arg(args, const char*)); //string
            index++;
        } else if (str[index] == '%' && str[index+1] == 'p') {
            globalCOM1->Print(to_string((uint64_t)va_arg(args, void*))); //address
            index++;
        } else {
            globalCOM1->Print(to_string(str[index])); //char
        }
        index++;
    }

    globalCOM1->Print("\n");
    va_end(args);
    Atomic::atomicUnlock(&mutexLog, 0);
}

void Logs::Error(const char * str, ...){
    va_list args;
    va_start(args, str);
    globalCOM1->Print(SerialRED);
    globalCOM1->Print("[Error] ");
    globalCOM1->Print(SerialReset);
    
    int index = 0;
    while(str[index] != 0) {
        if(str[index] == '%' && (str[index + 1] == 'd' || str[index + 1] == 'i')) {
            globalCOM1->Print(to_string(va_arg(args, int))); //int or decimal
            index++;
        } else if (str[index] == '%' && str[index+1] == 'u') {
            globalCOM1->Print(to_string(va_arg(args, uint64_t))); //uint
            index++;
        } else if (str[index] == '%' && (str[index+1] == 'x' || str[index+1] == 'X')) {
            uint64_t val = va_arg(args, uint64_t); //hex
            if(val > uint32_Limit)
                globalCOM1->Print(to_hstring(val)); 
            else if (val > uint16_Limit) 
                globalCOM1->Print(to_hstring((uint32_t)val));
            else if (val > uint8_Limit) 
                globalCOM1->Print(to_hstring((uint16_t)val));
            else
                globalCOM1->Print(to_hstring((uint8_t)val));                
            index++;    
        } else if (str[index] == '%' && (str[index + 1] == 'f' || str[index+1] == 'F')) {
            globalCOM1->Print(to_string(va_arg(args, double))); //float
            index++;
        } else if (str[index] == '%' && str[index+1] == 'c' && str[index+2] != 'o') {
            globalCOM1->Print(to_string(va_arg(args, int))); //char
            index++;
        } else if (str[index] == '%' && str[index+1] == 's') {
            globalCOM1->Print(va_arg(args, const char*)); //string
            index++;
        } else if (str[index] == '%' && str[index+1] == 'p') {
            globalCOM1->Print(to_string((uint64_t)va_arg(args, void*))); //address
            index++;
        } else {
            globalCOM1->Print(to_string(str[index])); //char
        }
        index++;
    }

    globalCOM1->Print("\n");
    va_end(args);
}
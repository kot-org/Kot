#include <logs/logs.h>

Logs* globalLogs;
static uint64_t mutexLog;

void Logs::Message(const char* str, ...){
    Atomic::atomicAcquire(&mutexLog, 0);

    va_list args;
    va_start(args, str);
    
    int index = 0;
    while(str[index] != 0) {
        if(str[index] == '%' && (str[index + 1] == 'd' || str[index + 1] == 'i')) {
            SerialPort::Print(to_string(va_arg(args, int))); //int or decimal
            index++;
        } else if (str[index] == '%' && str[index+1] == 'u') {
            SerialPort::Print(to_string(va_arg(args, uint64_t))); //uint
            index++;
        } else if (str[index] == '%' && (str[index+1] == 'x' || str[index+1] == 'X')) {
            uint64_t val = va_arg(args, uint64_t); //hex
            if(val > uint32_Limit)
                SerialPort::Print(to_hstring(val)); 
            else if (val > uint16_Limit) 
                SerialPort::Print(to_hstring((uint32_t)val));
            else if (val > uint8_Limit) 
                SerialPort::Print(to_hstring((uint16_t)val));
            else
                SerialPort::Print(to_hstring((uint8_t)val));                
            index++;    
        } else if (str[index] == '%' && (str[index + 1] == 'f' || str[index+1] == 'F')) {
            SerialPort::Print(to_string(va_arg(args, double))); //float
            index++;
        } else if (str[index] == '%' && str[index+1] == 'c' && str[index+2] != 'o') {
            SerialPort::Print(to_string(va_arg(args, int))); //char
            index++;
        } else if (str[index] == '%' && str[index+1] == 's') {
            SerialPort::Print(va_arg(args, const char*)); //string
            index++;
        } else if (str[index] == '%' && str[index+1] == 'p') {
            SerialPort::Print(to_string((uint64_t)va_arg(args, uintptr_t))); //address
            index++;
        } else {
            SerialPort::Print(to_string(str[index])); //char
        }
        index++;
    }

    SerialPort::Print("\n");
    va_end(args);
    Atomic::atomicUnlock(&mutexLog, 0);
}   

void Logs::Successful(const char* str, ...){
    Atomic::atomicAcquire(&mutexLog, 0);

    va_list args;
    va_start(args, str);
    
    SerialPort::Print(SerialGREEN);
    SerialPort::Print("[OK] ");
    SerialPort::Print(SerialReset);
    
    int index = 0;
    while(str[index] != 0) {
        if(str[index] == '%' && (str[index + 1] == 'd' || str[index + 1] == 'i')) {
            SerialPort::Print(to_string(va_arg(args, int))); //int or decimal
            index++;
        } else if (str[index] == '%' && str[index+1] == 'u') {
            SerialPort::Print(to_string(va_arg(args, uint64_t))); //uint
            index++;
        } else if (str[index] == '%' && (str[index+1] == 'x' || str[index+1] == 'X')) {
            uint64_t val = va_arg(args, uint64_t); //hex
            if(val > uint32_Limit)
                SerialPort::Print(to_hstring(val)); 
            else if (val > uint16_Limit) 
                SerialPort::Print(to_hstring((uint32_t)val));
            else if (val > uint8_Limit) 
                SerialPort::Print(to_hstring((uint16_t)val));
            else
                SerialPort::Print(to_hstring((uint8_t)val));                
            index++;    
        } else if (str[index] == '%' && (str[index + 1] == 'f' || str[index+1] == 'F')) {
            SerialPort::Print(to_string(va_arg(args, double))); //float
            index++;
        } else if (str[index] == '%' && str[index+1] == 'c' && str[index+2] != 'o') {
            SerialPort::Print(to_string(va_arg(args, int))); //char
            index++;
        } else if (str[index] == '%' && str[index+1] == 's') {
            SerialPort::Print(va_arg(args, const char*)); //string
            index++;
        } else if (str[index] == '%' && str[index+1] == 'p') {
            SerialPort::Print(to_string((uint64_t)va_arg(args, uintptr_t))); //address
            index++;
        } else {
            SerialPort::Print(to_string(str[index])); //char
        }
        index++;
    }

    SerialPort::Print("\n");
    va_end(args);
    Atomic::atomicUnlock(&mutexLog, 0);
}

void Logs::Warning(const char* str, ...){
    va_list args;
    va_start(args, str);
    
    SerialPort::Print(SerialYELLOW);
    SerialPort::Print("[Warning] ");
    SerialPort::Print(SerialReset);
    
    int index = 0;
    while(str[index] != 0) {
        if(str[index] == '%' && (str[index + 1] == 'd' || str[index + 1] == 'i')) {
            SerialPort::Print(to_string(va_arg(args, int))); //int or decimal
            index++;
        } else if (str[index] == '%' && str[index+1] == 'u') {
            SerialPort::Print(to_string(va_arg(args, uint64_t))); //uint
            index++;
        } else if (str[index] == '%' && (str[index+1] == 'x' || str[index+1] == 'X')) {
            uint64_t val = va_arg(args, uint64_t); //hex
            if(val > uint32_Limit)
                SerialPort::Print(to_hstring(val)); 
            else if (val > uint16_Limit) 
                SerialPort::Print(to_hstring((uint32_t)val));
            else if (val > uint8_Limit) 
                SerialPort::Print(to_hstring((uint16_t)val));
            else
                SerialPort::Print(to_hstring((uint8_t)val));                
            index++;    
        } else if (str[index] == '%' && (str[index + 1] == 'f' || str[index+1] == 'F')) {
            SerialPort::Print(to_string(va_arg(args, double))); //float
            index++;
        } else if (str[index] == '%' && str[index+1] == 'c' && str[index+2] != 'o') {
            SerialPort::Print(to_string(va_arg(args, int))); //char
            index++;
        } else if (str[index] == '%' && str[index+1] == 's') {
            SerialPort::Print(va_arg(args, const char*)); //string
            index++;
        } else if (str[index] == '%' && str[index+1] == 'p') {
            SerialPort::Print(to_string((uint64_t)va_arg(args, uintptr_t))); //address
            index++;
        } else {
            SerialPort::Print(to_string(str[index])); //char
        }
        index++;
    }

    SerialPort::Print("\n");
    va_end(args);
}

void Logs::Error(const char * str, ...){
    va_list args;
    va_start(args, str);
    SerialPort::Print(SerialRED);
    SerialPort::Print("[Error] ");
    SerialPort::Print(SerialReset);
    
    int index = 0;
    while(str[index] != 0) {
        if(str[index] == '%' && (str[index + 1] == 'd' || str[index + 1] == 'i')) {
            SerialPort::Print(to_string(va_arg(args, int))); //int or decimal
            index++;
        } else if (str[index] == '%' && str[index+1] == 'u') {
            SerialPort::Print(to_string(va_arg(args, uint64_t))); //uint
            index++;
        } else if (str[index] == '%' && (str[index+1] == 'x' || str[index+1] == 'X')) {
            uint64_t val = va_arg(args, uint64_t); //hex
            if(val > uint32_Limit)
                SerialPort::Print(to_hstring(val)); 
            else if (val > uint16_Limit) 
                SerialPort::Print(to_hstring((uint32_t)val));
            else if (val > uint8_Limit) 
                SerialPort::Print(to_hstring((uint16_t)val));
            else
                SerialPort::Print(to_hstring((uint8_t)val));                
            index++;    
        } else if (str[index] == '%' && (str[index + 1] == 'f' || str[index+1] == 'F')) {
            SerialPort::Print(to_string(va_arg(args, double))); //float
            index++;
        } else if (str[index] == '%' && str[index+1] == 'c' && str[index+2] != 'o') {
            SerialPort::Print(to_string(va_arg(args, int))); //char
            index++;
        } else if (str[index] == '%' && str[index+1] == 's') {
            SerialPort::Print(va_arg(args, const char*)); //string
            index++;
        } else if (str[index] == '%' && str[index+1] == 'p') {
            SerialPort::Print(to_string((uint64_t)va_arg(args, uintptr_t))); //address
            index++;
        } else {
            SerialPort::Print(to_string(str[index])); //char
        }
        index++;
    }

    SerialPort::Print("\n");
    va_end(args);
}

void Logs::PrintRegisters(RegistersLog* registers){
    globalLogs->Message("Rax : %x Rbx : %x Rcx : %x Rdx : %x Rsi : %x Rdi : %x Rbp : %x", registers->rax, registers->rbx, registers->rcx, registers->rdx, registers->rsi, registers->rdi, registers->rbp);
    globalLogs->Message("R8 : %x R9 : %x R10 : %x R11 : %x R12 : %x R13 : %x R14 : %x R15 : %x", registers->r8, registers->r9, registers->r10, registers->r11, registers->r12, registers->r13, registers->r14, registers->r15);
    globalLogs->Message("Rflags: %x Rip: %x Ss: %x Cs: %x Rsp: %x", registers->rflags, registers->rip, registers->ss, registers->cs, registers->rsp);
}
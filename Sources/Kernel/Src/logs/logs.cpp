#include <logs/logs.h>

static locker_t MutexLog;

void Message(const char* str, ...){
    AtomicAquire(&MutexLog);
    va_list args;
    va_start(args, str);

    SerialPort::Print(SerialCYAN);
    SerialPort::Print("[*] ");
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
            SerialPort::Print(to_string((uint64_t)va_arg(args, void*))); //address
            index++;
        } else {
            SerialPort::Print(to_string(str[index])); //char
        }
        index++;
    }

    SerialPort::Print("\n");
    va_end(args);
    AtomicRelease(&MutexLog);
}   

void MessageProcess(const char* str, uint64_t charNum, uint64_t PID, uint64_t PPID, uint64_t TID){
    AtomicAquire(&MutexLog);

    SerialPort::Print(SerialCYAN);
    SerialPort::Print("[*]");
    SerialPort::Print(SerialReset);
    SerialPort::Printf("[Process %x:%x:%x]", PID, PPID, TID);
    SerialPort::Print(str, charNum);
    

    SerialPort::Print("\n");
    AtomicRelease(&MutexLog);
}   

void Successful(const char* str, ...){
    AtomicAquire(&MutexLog);
    va_list args;
    va_start(args, str);
    
    SerialPort::Print(SerialGREEN);
    SerialPort::Print("[$]");
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
            SerialPort::Print(to_string((uint64_t)va_arg(args, void*))); //address
            index++;
        } else {
            SerialPort::Print(to_string(str[index])); //char
        }
        index++;
    }

    SerialPort::Print("\n");
    va_end(args);
    AtomicRelease(&MutexLog);
}

void Warning(const char* str, ...){
    AtomicAquire(&MutexLog);
    va_list args;
    va_start(args, str);
    
    SerialPort::Print(SerialYELLOW);
    SerialPort::Print("[%]");
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
            SerialPort::Print(to_string((uint64_t)va_arg(args, void*))); //address
            index++;
        } else {
            SerialPort::Print(to_string(str[index])); //char
        }
        index++;
    }

    SerialPort::Print("\n");
    va_end(args);
    AtomicRelease(&MutexLog);
}

void Error(const char * str, ...){
    AtomicAquire(&MutexLog);
    va_list args;
    va_start(args, str);
    SerialPort::Print(SerialRED);
    SerialPort::Print("[~]");
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
            SerialPort::Print(to_string((uint64_t)va_arg(args, void*))); //address
            index++;
        } else {
            SerialPort::Print(to_string(str[index])); //char
        }
        index++;
    }

    SerialPort::Print("\n");
    va_end(args);
    AtomicRelease(&MutexLog);
}

void PrintRegisters(ContextStack* registers){
    Message("Rax : %x Rbx : %x Rcx : %x Rdx : %x Rsi : %x Rdi : %x Rbp : %x", registers->GlobalPurpose, registers->rbx, registers->arg3, registers->arg2, registers->arg1, registers->arg0, registers->rbp);
    Message("R8 : %x R9 : %x R10 : %x R11 : %x R12 : %x R13 : %x R14 : %x R15 : %x", registers->arg5, registers->arg4, registers->r10, registers->r11, registers->r12, registers->r13, registers->r14, registers->r15);
    Message("Rflags: %x Rip: %x Ss: %x Cs: %x Rsp: %x", registers->rflags, registers->rip, registers->ss, registers->cs, registers->rsp);
}
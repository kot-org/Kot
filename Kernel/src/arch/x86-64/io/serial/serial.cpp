#include "serial.h"

SerialPort* globalCOM1;

void SerialPort::Initialize() {
    IoWrite8(COM1 + 1, 0x00);    // Disable all interrupts
    IoWrite8(COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    IoWrite8(COM1 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    IoWrite8(COM1 + 1, 0x00);    //                  (hi byte)
    IoWrite8(COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
    IoWrite8(COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    IoWrite8(COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
    IoWrite8(COM1 + 4, 0x0F);    // Set in normal mode
    globalLogs->Successful("Serial initialize");
}

void SerialPort::Write(char chr) {
    IoWrite8(COM1, chr);
}

void SerialPort::Print(const char* chr) {
    for(int i = 0;chr[i] != '\0';i++){
        Write(chr[i]);
        if(chr[i] == '\n') Write('\r');   
    }
}

void SerialPort::Printf(const char* str, ...){
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

    va_end(args);
}

void SerialPort::ClearMonitor() {
    Write(27);
    Print("[2J");
    Write(27);
    Print("[H");
}
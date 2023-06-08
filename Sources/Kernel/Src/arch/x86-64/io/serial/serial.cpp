#include <arch/x86-64/io/serial/serial.h>

namespace SerialPort{

    void Initialize() {
        IoWrite8(COM1 + 1, 0x00);    // Disable all interrupts
        IoWrite8(COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
        IoWrite8(COM1 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
        IoWrite8(COM1 + 1, 0x00);    //                  (hi byte)
        IoWrite8(COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
        IoWrite8(COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
        IoWrite8(COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
        IoWrite8(COM1 + 4, 0x0F);    // Set in normal mode
        Successful("Serial initialized");
    }

    void Write(char chr) {
        #ifdef DEBUG_WITH_BOCHS
            IoWrite8(BOCHSLOG, chr);
        #endif
        #ifdef USE_COM_1
            IoWrite8(COM1, chr);
        #endif
    }

    void Print(const char* chr) {
        for(uint64_t i = 0; chr[i] != '\0'; i++){
            Write(chr[i]);
            if(chr[i] == '\n') Write('\r');   
        }
    }

    void Print(const char* chr, uint64_t charNum) {
        for(uint64_t i = 0; i < charNum; i++){
            Write(chr[i]);
            if(chr[i] == '\n') Write('\r');   
        }
    }

    void Printf(const char* str, ...){
        va_list args;
        va_start(args, str);
        
        int index = 0;
        while(str[index] != 0) {
            if(str[index] == '%' && (str[index + 1] == 'd' || str[index + 1] == 'i')) {
                Print(to_string(va_arg(args, int))); //int or decimal
                index++;
            } else if (str[index] == '%' && str[index+1] == 'u') {
                Print(to_string(va_arg(args, uint64_t))); //uint
                index++;
            } else if (str[index] == '%' && (str[index+1] == 'x' || str[index+1] == 'X')) {
                uint64_t val = va_arg(args, uint64_t); //hex
                if(val > uint32_Limit)
                    Print(to_hstring(val)); 
                else if (val > uint16_Limit) 
                    Print(to_hstring((uint32_t)val));
                else if (val > uint8_Limit) 
                    Print(to_hstring((uint16_t)val));
                else
                    Print(to_hstring((uint8_t)val));                
                index++;    
            } else if (str[index] == '%' && (str[index + 1] == 'f' || str[index+1] == 'F')) {
                Print(to_string(va_arg(args, double))); //float
                index++;
            } else if (str[index] == '%' && str[index+1] == 'c' && str[index+2] != 'o') {
                Print(to_string(va_arg(args, int))); //char
                index++;
            } else if (str[index] == '%' && str[index+1] == 's') {
                Print(va_arg(args, const char*)); //string
                index++;
            } else if (str[index] == '%' && str[index+1] == 'p') {
                Print(to_string((uint64_t)va_arg(args, void*))); //address
                index++;
            } else {
                Print(to_string(str[index])); //char
            }
            index++;
        }

        va_end(args);
    }

    void ClearMonitor() {
        Write(27);
        Print("[2J");
        Write(27);
        Print("[H");
    }

}
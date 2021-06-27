#include "printf.h"

void printf(const char* str, ...) {
    va_list args;
    va_start(args, str);
    
    int index = 0;
    while(str[index] != 0) {
        if(str[index] == '%' && (str[index + 1] == 'd' || str[index + 1] == 'i')) {
            globalGraphics->Print(to_string(va_arg(args, int))); //int or decimal
            index++;
        } else if (str[index] == '%' && str[index+1] == 'u') {
            globalGraphics->Print(to_string(va_arg(args, uint64_t))); //uint
            index++;
        } else if (str[index] == '%' && (str[index+1] == 'x' || str[index+1] == 'X')) {
            uint64_t val = va_arg(args, uint64_t); //hex
            if(val > uint32_Limit)
                globalGraphics->Print(to_hstring(val)); 
            else if (val > uint16_Limit) 
                globalGraphics->Print(to_hstring((uint32_t)val));
            else if (val > uint8_Limit) 
                globalGraphics->Print(to_hstring((uint16_t)val));
            else
                globalGraphics->Print(to_hstring((uint8_t)val));                
            index++;    
        } else if (str[index] == '%' && (str[index + 1] == 'f' || str[index+1] == 'F')) {
            globalGraphics->Print(to_string(va_arg(args, double))); //float
            index++;
        } else if (str[index] == '%' && str[index+1] == 'c' && str[index+2] != 'o') {
            globalGraphics->Print(to_string(va_arg(args, int))); //char
            index++;
        } else if (str[index] == '%' && str[index+1] == 's') {
            globalGraphics->Print(va_arg(args, const char*)); //string
            index++;
        } else if (str[index] == '%' && str[index+1] == 'p') {
            globalGraphics->Print(to_string((uint64_t)va_arg(args, void*))); //address
            index++;
        } else if (str[index] == '%' && str[index+1] == 'k') {
            globalGraphics->Color = va_arg (args, uint32_t); //colour
            index++;
        } else if (str[index] == '\n'){
            globalGraphics->Next();
        }else {
            globalGraphics->Print(to_string(str[index])); //char
        }
        index++;
    }

    va_end(args);
}
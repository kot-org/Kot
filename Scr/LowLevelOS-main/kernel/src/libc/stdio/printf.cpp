#include "printf.h"

void printf(const char* str, ...) {
    va_list args;
    va_start(args,str);
    
    int index = 0;
    while(str[index] != 0) {
        if(str[index] == '%' && (str[index+1] == 'd' || str[index+1] == 'i')) {
            GlobalDisplay->puts(inttostr(va_arg (args, int))); //int or decimal
            index++;
        } else if (str[index] == '%' && str[index+1] == 'u') {
            GlobalDisplay->puts(inttostr(va_arg (args, uint64_t))); //uint
            index++;
        } else if (str[index] == '%' && (str[index+1] == 'x' || str[index+1] == 'X')) {
            uint64_t val = va_arg (args, uint64_t); //hex
            if(val > uint32_Limit)
                GlobalDisplay->puts(inttohstr(val)); 
            else if (val > uint16_Limit) 
                GlobalDisplay->puts(inttohstr((uint32_t)val));
            else if (val > uint8_Limit) 
                GlobalDisplay->puts(inttohstr((uint16_t)val));
            else
                GlobalDisplay->puts(inttohstr((uint8_t)val));                
            index++;    
        } else if (str[index] == '%' && (str[index+1] == 'f' || str[index+1] == 'F')) {
            GlobalDisplay->puts(inttostr(va_arg (args, double))); //float
            index++;
        } else if (str[index] == '%' && str[index+1] == 'c' && str[index+2] != 'o') {
            GlobalDisplay->puts(chartostr(va_arg (args, int))); //char
            index++;
        } else if (str[index] == '%' && str[index+1] == 's') {
            GlobalDisplay->puts(va_arg (args, const char*)); //string
            index++;
        } else if (str[index] == '%' && str[index+1] == 'p') {
            GlobalDisplay->puts(inttostr((uint64_t)va_arg (args, void*))); //address
            index++;
        } else if (str[index] == '%' && str[index+1] == 'c' && str[index+2] == 'o') {
            GlobalDisplay->colour = va_arg (args, int); //colour
            index+=2;
        } else {
            GlobalDisplay->puts(chartostr(str[index])); //char
        }
        index++;
    }

    va_end(args);
}
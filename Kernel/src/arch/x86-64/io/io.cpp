#include <arch/x86-64/io/io.h>

bool IO_IN(uint8_t size, uint16_t port, uint32_t data){
    switch(size){
        case 8:
            IoWrite8(port, (uint8_t)data);
            return true;
        case 16:
            IoWrite16(port, (uint16_t)data);
            return true;
        case 32:
            IoWrite32(port, (uint32_t)data);
            return true;
        default:
            return false;
    }
}

uint32_t IO_OUT(uint8_t size, uint16_t port){
    switch(size){
        case 8:
            return (uint32_t)IoRead8(port);
        case 16:
            return (uint32_t)IoRead16(port);
        case 32:
            return (uint32_t)IoRead32(port);
        default:
            return 0;
    }  
}
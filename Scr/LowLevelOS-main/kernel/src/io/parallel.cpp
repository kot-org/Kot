#include "parallel.h"

uint8_t Parallel::getStatusFlag(int flag) {
    uint8_t result = 1;
    while (flag != 0) {
        result *= 2;
        --flag;
    }
    return inportb(LPT1_STATUS) & result;
}

uint8_t Parallel::getControlFlag(int flag) {
    uint8_t result = 1;
    while (flag != 0) {
        result *= 2;
        --flag;
    }
    return inportb(LPT1_CONTROL) & result;  
}

void Parallel::delay() {
    for(int i = 0;i<5000000;i++){}
}

void Parallel::waitPrinterReady() {
    //         ERROR                 ACK                 BUSY
    while(getStatusFlag(3) || getStatusFlag(6) || getStatusFlag(7)) {delay();}
}

void Parallel::tellPrinterRead() {
    unsigned char c;
    c = inportb(LPT1_CONTROL);
    outportb(LPT1_CONTROL, c | 1);
    delay();
    outportb(LPT1_CONTROL, c);
}

void Parallel::writeChar(unsigned char ch) {
    waitPrinterReady();
    outportb(LPT1_DATA, ch);
    tellPrinterRead();
    waitPrinterReady();
}

void Parallel::writeChars(const char* ch) {
    for(int i = 0;ch[i] != '\0';i++) {
        writeChar(ch[i]);
    }
}
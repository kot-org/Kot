#include "pit.h"
#include "../io/ports.h"

double TimeSinceBoot = 0;
uint16_t Divisor = 65535;

void PITSleep(double seconds) {
    double start = TimeSinceBoot;
    while(TimeSinceBoot < start + seconds) {
        asm("hlt");
    }
}
void PITSleep(uint64_t ms) {
    PITSleep((double)ms / 1000);
}

void PITSetDivisor(uint16_t divisor) {
    if(divisor < 100) divisor = 100;
    Divisor = divisor;
    outportb(0x40,(uint8_t)(divisor & 0x00FF));
    io_wait();
    outportb(0x40,(uint8_t)((divisor & 0xFF00) >> 8));
}

uint64_t PITGetFrequency() {
    return PITBaseFrequency / Divisor;
}

void PITSetFrequency(uint64_t frequency) {
    PITSetDivisor(PITBaseFrequency / frequency);
}

void PITTick() {
    TimeSinceBoot += 1 / (double)PITGetFrequency();
}
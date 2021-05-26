#include "rtc.h"

void RealTimeClock::waitUpdate() {
    int old = getRegister(0x0);
    while(getRegister(0x0) == old);
}

int RealTimeClock::getUpdateInProgress() {
    IoWrite8(0x70, 0x0A);
    return (IoRead8(0x71) & 0x80);
}

unsigned char RealTimeClock::getRegister(int reg) {
    IoWrite8(0x70, reg);
    return (IoRead8(0x71));
}

uint32_t RealTimeClock::readSeconds() {
    while(getUpdateInProgress());
    int second = getRegister(0x0);
    return (second & 0x0F) + ((second / 16) * 10);
}

uint32_t RealTimeClock::readHours() {
    while(getUpdateInProgress());
    int second = getRegister(0x4);
    return (second & 0x0F) + ((second / 16) * 10);
}

uint32_t RealTimeClock::readMinutes() {
    while(getUpdateInProgress());
    int second = getRegister(0x2);
    return (second & 0x0F) + ((second / 16) * 10);
}

uint32_t RealTimeClock::readDay() {
    while(getUpdateInProgress());
    int second = getRegister(0x7);
    return (second & 0x0F) + ((second / 16) * 10);
}

uint32_t RealTimeClock::readMonth() {
    while(getUpdateInProgress());
    int second = getRegister(0x8);
    return (second & 0x0F) + ((second / 16) * 10);
}

uint32_t RealTimeClock::readYear() {
    while(getUpdateInProgress());
    int second = getRegister(0x9);
    return (second & 0x0F) + ((second / 16) * 10);
}

uint32_t RealTimeClock::readTime() {
    return readHours() * 3600 + readMinutes() * 60 + readSeconds();
}

void RealTimeClock::waitSeconds(uint32_t secs) {
    waitUpdate();
    int lastsec = readTime() + secs;
    while(lastsec != readTime());
}
#include <core/main.h>

#include <kot++/printf.h>

using namespace std;

/* PRIVATE */

uint8_t GetRTCReg(int reg) {
    IoWrite8(CMOS_ADDR, reg);
    return IoRead8(CMOS_DATA);
}

bool isUpdatingRTC() {
    return (GetRTCReg(RTC_REGA) & 0x80);
}

bool isHexRTC() {
    if(!(GetRTCReg(0xB) & 0x4))
        return true;
    return false;
}

int BCDToBIN(uint8_t value) {
    return ((value & 0x0F) + ((value / 16) * 10));
}

extern "C" int main(int argc, char* argv[]) {
    Printlog("[TIMER/RTC] Initialization ...");

    std::printf("test %f test", 5.1);
    
    Printlog("[TIMER/RTC] Driver initialized successfully");

    return KSUCCESS;
}

/* PUBLIC */

uint8_t GetSecond() {
    while(isUpdatingRTC());
    if(isHexRTC())
        return BCDToBIN(GetRTCReg(RTC_SECOND));
    return GetRTCReg(RTC_SECOND);
}

uint8_t GetMinute() {
    while(isUpdatingRTC());
    if(isHexRTC())
        return BCDToBIN(GetRTCReg(RTC_MINUTE));
    return GetRTCReg(RTC_MINUTE);
}

uint8_t GetHour() {
    while(isUpdatingRTC());
    if(isHexRTC())
        return BCDToBIN(GetRTCReg(RTC_HOUR));
    return GetRTCReg(RTC_HOUR);
}

uint8_t GetDayOfWeek() {
    while(isUpdatingRTC());
    if(isHexRTC())
        return BCDToBIN(GetRTCReg(RTC_DOW));
    return GetRTCReg(RTC_DOW);
}

uint8_t GetDayOfMonth() {
    while(isUpdatingRTC());
    if(isHexRTC())
        return BCDToBIN(GetRTCReg(RTC_DOM));
    return GetRTCReg(RTC_DOM);
}

uint8_t GetMonth() {
    while(isUpdatingRTC());
    if(isHexRTC())
        return BCDToBIN(GetRTCReg(RTC_MONTH));
    return GetRTCReg(RTC_MONTH);
}

uint8_t GetYear() {
    while(isUpdatingRTC());
    if(isHexRTC())
        return BCDToBIN(GetRTCReg(RTC_YEAR));
    return GetRTCReg(RTC_YEAR);
}

uint8_t GetCentury() {
    while(isUpdatingRTC());
    if(isHexRTC())
        return BCDToBIN(GetRTCReg(RTC_CENTURY));
    return GetRTCReg(RTC_CENTURY);
}
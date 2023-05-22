#include <core/main.h>

#include <kot++/printf.h>

using namespace std;

/* PRIVATE */

uint8_t GetRTCReg(int reg) {
    kot_IoWrite8(CMOS_ADDR, reg);
    return kot_IoRead8(CMOS_DATA);
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

/* PUBLIC */

uint8_t RTCGetSecond() {
    while(isUpdatingRTC());
    if(isHexRTC())
        return BCDToBIN(GetRTCReg(RTC_SECOND));
    return GetRTCReg(RTC_SECOND);
}

uint8_t RTCGetMinute() {
    while(isUpdatingRTC());
    if(isHexRTC())
        return BCDToBIN(GetRTCReg(RTC_MINUTE));
    return GetRTCReg(RTC_MINUTE);
}

uint8_t RTCGetHour() {
    while(isUpdatingRTC());
    if(isHexRTC())
        return BCDToBIN(GetRTCReg(RTC_HOUR));
    return GetRTCReg(RTC_HOUR);
}

uint8_t RTCGetDayOfWeek() {
    while(isUpdatingRTC());
    if(isHexRTC())
        return BCDToBIN(GetRTCReg(RTC_DOW));
    return GetRTCReg(RTC_DOW);
}

uint8_t RTCGetDayOfMonth() {
    while(isUpdatingRTC());
    if(isHexRTC())
        return BCDToBIN(GetRTCReg(RTC_DOM));
    return GetRTCReg(RTC_DOM);
}

uint8_t RTCGetMonth() {
    while(isUpdatingRTC());
    if(isHexRTC())
        return BCDToBIN(GetRTCReg(RTC_MONTH));
    return GetRTCReg(RTC_MONTH);
}

uint8_t RTCGetYear() {
    while(isUpdatingRTC());
    if(isHexRTC())
        return BCDToBIN(GetRTCReg(RTC_YEAR));
    return GetRTCReg(RTC_YEAR);
}

uint8_t RTCGetCentury() {
    while(isUpdatingRTC());
    if(isHexRTC())
        return BCDToBIN(GetRTCReg(RTC_CENTURY));
    return GetRTCReg(RTC_CENTURY);
}

extern uint64_t* TickPointer;

extern "C" int main(int argc, char* argv[]) {
    kot_Printlog("[TIMER/RTC] Initialization ...");

    

    kot_Printlog("[TIMER/RTC] Driver initialized successfully");

    uint64_t TimerState;
    kot_GetActualTick(&TimerState);

    kot_time_t* Time;
    kot_Srv_Time_SetTimePointerKey(&Time, true);

    while (true){
        Time->Year = RTCGetYear() + 2000;
        Time->Month = RTCGetMonth();
        Time->Day = RTCGetDayOfMonth();
        Time->WeekDay = RTCGetDayOfWeek();
        Time->Hour = RTCGetHour();
        Time->Minute = RTCGetMinute();
        Time->Second = RTCGetSecond();
        kot_SleepFromTick(&TimerState, 1000);
    }
    return KSUCCESS;
}
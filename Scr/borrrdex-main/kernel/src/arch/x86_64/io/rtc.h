#pragma once

#include <cstdint>

extern uint8_t century_register;

typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t weekday;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} datetime_t;

void rtc_init();
datetime_t* rtc_read(datetime_t* dt);

void rtc_set_interrupt_frequency(uint8_t frequency);
uint8_t rtc_get_interrupt_frequency();
uint16_t rtc_get_interrupt_frequency_hz();

typedef void(*RTCCallback)(datetime_t* dt, void* context);
typedef struct rtc_chain rtc_chain_t;

struct rtc_chain {
    RTCCallback cb;
    void* context;
    rtc_chain_t* next;
};

void register_rtc_cb(rtc_chain_t* chainEntry);
void unregister_rtc_cb(rtc_chain_t* chainEntry);
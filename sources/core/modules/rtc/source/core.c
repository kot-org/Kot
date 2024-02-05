#include <core.h>

static uint8_t get_rtc_reg(int reg) {
    io_write8(CMOS_ADDR, reg);
    return io_read8(CMOS_DATA);
}

static bool is_updating_rtc(void) {
    return (get_rtc_reg(RTC_REGA) & 0x80);
}

static bool is_hex_rtc(void) {
    if(!(get_rtc_reg(0xb) & 0x4))
        return true;
    return false;
}

static int bcd_to_bin(uint8_t value) {
    return ((value & 0x0f) + ((value / 16) * 10));
}

static uint8_t rtc_get_century(void) {
    while(is_updating_rtc());
    if(is_hex_rtc())
        return bcd_to_bin(get_rtc_reg(RTC_CENTURY));
    return get_rtc_reg(RTC_CENTURY);
}

uint8_t rtc_get_second(void) {
    while(is_updating_rtc());
    if(is_hex_rtc())
        return bcd_to_bin(get_rtc_reg(RTC_SECOND));
    return get_rtc_reg(RTC_SECOND);
}

uint8_t rtc_get_minute(void) {
    while(is_updating_rtc());
    if(is_hex_rtc())
        return bcd_to_bin(get_rtc_reg(RTC_MINUTE));
    return get_rtc_reg(RTC_MINUTE);
}

uint8_t rtc_get_hour(void) {
    while(is_updating_rtc());
    if(is_hex_rtc())
        return bcd_to_bin(get_rtc_reg(RTC_HOUR));
    return get_rtc_reg(RTC_HOUR);
}

uint8_t rtc_get_day_of_week(void) {
    while(is_updating_rtc());
    if(is_hex_rtc())
        return bcd_to_bin(get_rtc_reg(RTC_DOW));
    return get_rtc_reg(RTC_DOW);
}

uint8_t rtc_get_day_of_month(void) {
    while(is_updating_rtc());
    if(is_hex_rtc())
        return bcd_to_bin(get_rtc_reg(RTC_DOM));
    return get_rtc_reg(RTC_DOM);
}

uint8_t rtc_get_month(void) {
    while(is_updating_rtc());
    if(is_hex_rtc())
        return bcd_to_bin(get_rtc_reg(RTC_MONTH));
    return get_rtc_reg(RTC_MONTH);
}

uint16_t rtc_get_year(void) {
    while(is_updating_rtc());
    if(is_hex_rtc())
        return bcd_to_bin(get_rtc_reg(RTC_YEAR)) + RTC_YEAR_BASE;
    return get_rtc_reg(RTC_YEAR) + RTC_YEAR_BASE;
}
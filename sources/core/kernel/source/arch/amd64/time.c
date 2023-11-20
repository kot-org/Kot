#include <impl/time.h>
#include <arch/include.h>
#include <global/modules.h>
#include ARCH_INCLUDE(hpet.h)

static time_handler_t early_time_handler;

void time_init(void){
    early_time_handler.get_current_second = kernel_get_current_second;
    early_time_handler.get_current_minute = kernel_get_current_minute;
    early_time_handler.get_current_hour = kernel_get_current_hour;
    early_time_handler.get_current_month_day = kernel_get_current_month_day;
    early_time_handler.get_current_week_day = kernel_get_current_week_day;
    early_time_handler.get_current_month = kernel_get_current_month;
    early_time_handler.get_current_year = kernel_get_current_year;
    early_time_handler.sleep = kernel_sleep;
    early_time_handler.sleep_us = kernel_sleep_us;
    early_time_handler.get_current_us = kernel_get_current_us;
    time_handler = &early_time_handler;
}

uint8_t kernel_get_current_second(void){
    return 0;
}

uint8_t kernel_get_current_minute(void){
    return 0;
}

uint8_t kernel_get_current_hour(void){
    return 0;
}

uint8_t kernel_get_current_month_day(void){
    return 0;
}

uint8_t kernel_get_current_week_day(void){
    return 0;
}

uint8_t kernel_get_current_month(void){
    return 0;
}

uint16_t kernel_get_current_year(void){
    return 0;
}

int kernel_sleep(int seconds){
    hpet_sleep(seconds * 1000);
    return 0;
}

int kernel_sleep_us(us_t us){
    hpet_sleep(us);
    return 0;
}

us_t kernel_get_current_us(void){
    return hpet_get_current_time();
}
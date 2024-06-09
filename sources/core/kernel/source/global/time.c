#include <lib/time.h>
#include <global/time.h>
#include <global/file.h>
#include <global/modules.h>
#include <lib/modules/time.h>

uint8_t get_current_second(void){
    return time_handler->get_current_second();
}

uint8_t get_current_minute(void){
    return time_handler->get_current_minute();
}

uint8_t get_current_hour(void){
    return time_handler->get_current_hour();
}

uint8_t get_current_month_day(void){
    return time_handler->get_current_month_day();
}

uint8_t get_current_week_day(void){
    return time_handler->get_current_week_day();
}

uint8_t get_current_month(void){
    return time_handler->get_current_month();
}

uint16_t get_current_year(void){
    return time_handler->get_current_year();
}

int sleep(int seconds){
    return time_handler->sleep(seconds);
}

int sleep_us(us_t us){
    return time_handler->sleep_us(us);
}

us_t get_current_us(void){
    return time_handler->get_current_us();
}

time_t get_unix_time(us_t current_us){
    static time_t time_at_start = 0;
    if(time_at_start == 0){
        struct tm time_info;

        time_info.tm_year = get_current_year() - TM_YEAR_BASE;
        time_info.tm_mon = get_current_month();
        time_info.tm_mday = get_current_month_day();
        time_info.tm_hour = get_current_hour();
        time_info.tm_min = get_current_minute();
        time_info.tm_sec = get_current_second();

        time_at_start = mktime(&time_info) - TIME_CONVERT_MICROSECOND_TO_SECOND(current_us);
    }

    return time_at_start + TIME_CONVERT_MICROSECOND_TO_SECOND(current_us);
}
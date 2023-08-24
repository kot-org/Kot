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

uint8_t get_current_day(void){
    return time_handler->get_current_day();
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

int sleep_ms(ms_t ms){
    return time_handler->sleep_ms(ms);
}

ms_t get_current_ms(void){
    return time_handler->get_current_ms();
}
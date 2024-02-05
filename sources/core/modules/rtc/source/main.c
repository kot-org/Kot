#include <lib/log.h>
#include <errno.h>
#include <global/modules.h>

#include <core.h>

#define MODULE_NAME "rtc"

#include <core.c>

int init(int argc, char* args[]){
    #if defined(__x86_64__) || defined(_M_X64) || defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
    log_printf("[module/"MODULE_NAME"] loading start\n");
    
    time_handler->get_current_second = &rtc_get_second;
    time_handler->get_current_minute = &rtc_get_minute;
    time_handler->get_current_hour = &rtc_get_hour;
    time_handler->get_current_month_day = &rtc_get_day_of_month;
    time_handler->get_current_week_day = &rtc_get_day_of_week;
    time_handler->get_current_month = &rtc_get_month;
    time_handler->get_current_year = &rtc_get_year;

    log_printf("[module/"MODULE_NAME"] loading end\n");    
    return 0;
    #else
    log_printf("[module/"MODULE_NAME"] has no support for this arch\n");    
    return EINVAL;
    #endif
}

int fini(void){
    return 0;
}

module_metadata_t module_metadata = {
    &init,
    &fini,
    MODULE_TYPE_TIME,
    MODULE_NAME
};

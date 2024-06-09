#ifndef _GLOBAL_TIME_H
#define _GLOBAL_TIME_H 1

#include <stdint.h>
#include <stddef.h>
#include <lib/modules/time.h>

#define TIME_GET_MICROSECOND_UNDER_SECOND(t)        (t % 1000000) 
#define TIME_CONVERT_SECOND_TO_MICROSECOND(t)       (t * 1000000)
#define TIME_CONVERT_MICROSECOND_TO_SECOND(t)       (t / 1000000)
#define TIME_CONVERT_MICROSECOND_TO_NANOSECOND(t)   (t * 1000)
#define TIME_CONVERT_NANOSECOND_TO_MICROSECOND(t)   (t / 1000)
#define TIME_CONVERT_MICROSECOND_TO_FEMOSECOND(t)   (t * 1000000000)
#define TIME_CONVERT_FEMOSECOND_TO_MICROSECOND(t)   (t / 1000000000)


uint8_t get_current_second(void);
uint8_t get_current_minute(void);
uint8_t get_current_hour(void);

uint8_t get_current_month_day(void);
uint8_t get_current_week_day(void);
uint8_t get_current_month(void);
uint16_t get_current_year(void);

int sleep(int seconds);
int sleep_us(us_t us);

us_t get_current_us(void);

time_t get_unix_time(us_t current_us);

#endif // _GLOBAL_TIME_H
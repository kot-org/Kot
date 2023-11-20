#ifndef _IMPL_TIME_H
#define _IMPL_TIME_H 1

#include <stdint.h>

typedef uint64_t us_t; // microsecond

void time_init(void);

uint8_t kernel_get_current_second(void);
uint8_t kernel_get_current_minute(void);
uint8_t kernel_get_current_hour(void);
uint8_t kernel_get_current_month_day(void);
uint8_t kernel_get_current_week_day(void);
uint8_t kernel_get_current_month(void);
uint16_t kernel_get_current_year(void);
int kernel_sleep_us(us_t us);
int kernel_sleep(int seconds);
us_t kernel_get_current_us(void);

#endif // _SERIAL_H

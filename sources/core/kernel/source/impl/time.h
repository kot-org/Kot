#ifndef _IMPL_TIME_H
#define _IMPL_TIME_H 1

#include <stdint.h>

typedef uint64_t ms_t; // microsecond

void time_init(void);

uint8_t kernel_get_current_second(void);
uint8_t kernel_get_current_minute(void);
uint8_t kernel_get_current_hour(void);
uint8_t kernel_get_current_day(void);
uint8_t kernel_get_current_month(void);
uint16_t kernel_get_current_year(void);
int kernel_sleep_ms(ms_t ms);
int kernel_sleep(int seconds);
ms_t kernel_get_current_ms(void);

#endif // _SERIAL_H

#ifndef _GLOBAL_TIME_H
#define _GLOBAL_TIME_H 1

#include <stdint.h>
#include <stddef.h>
#include <lib/modules/time.h>

uint8_t get_current_second(void);
uint8_t get_current_minute(void);
uint8_t get_current_hour(void);

uint8_t get_current_day(void);
uint8_t get_current_month(void);
uint16_t get_current_year(void);

int sleep(int seconds);
int sleep_ms(ms_t ms);

ms_t get_current_ms(void);

#endif // _GLOBAL_TIME_H
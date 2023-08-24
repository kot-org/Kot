#ifndef _MODULES_TIME_H
#define _MODULES_TIME_H 1

#include <impl/time.h>
#include <lib/modules/file.h>

typedef struct{
    uint8_t (*get_current_second)(void);
    uint8_t (*get_current_minute)(void);
    uint8_t (*get_current_hour)(void);

    uint8_t (*get_current_day)(void);
    uint8_t (*get_current_month)(void);
    uint16_t (*get_current_year)(void);

    int (*sleep)(int);
    int (*sleep_ms)(ms_t ms);

    ms_t (*get_current_ms)(void);
} time_handler_t;

#endif // _MODULES_TIME_H
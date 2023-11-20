#ifndef LIB_MODULES_TIME_H
#define LIB_MODULES_TIME_H 1

#include <impl/time.h>
#include <lib/modules/file.h>

typedef struct{
    uint8_t (*get_current_second)(void);
    uint8_t (*get_current_minute)(void);
    uint8_t (*get_current_hour)(void);

    uint8_t (*get_current_month_day)(void);
    uint8_t (*get_current_week_day)(void);
    uint8_t (*get_current_month)(void);
    uint16_t (*get_current_year)(void);

    int (*sleep)(int);
    int (*sleep_us)(us_t us);

    us_t (*get_current_us)(void);
} time_handler_t;

#endif // LIB_MODULES_TIME_H
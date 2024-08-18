#include <lib/time.h>

#define SECS_PER_MIN            60
#define SECS_PER_HOUR           3600
#define SECS_PER_DAY            86400
#define MONTHS_PER_YEAR         12
#define EPOCH_YEAR              1970
#define IS_LEAP_YEAR(year)  (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0))

static int days_per_month[2][MONTHS_PER_YEAR] = {
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

static int days_per_year[2] = {365, 366};

time_t mktime(struct tm *timeinfo){
    time_t ts = 0;

    uint8_t years = 0;
    uint8_t leap_years = 0;
    for(uint16_t y_k = 1970; y_k < timeinfo->tm_year + TM_YEAR_BASE; y_k++){
        if (IS_LEAP_YEAR(y_k)){
            leap_years++;
        }
        else{
            years++;
        }
    }
    ts += ((years * days_per_year[0]) + (leap_years * days_per_year[1])) * SECS_PER_DAY;

    uint8_t year_index = (IS_LEAP_YEAR(timeinfo->tm_year + TM_YEAR_BASE)) ? 1 : 0;
    for(uint8_t mo_k = 0; mo_k < (timeinfo->tm_mon - 1); mo_k++){
        ts += days_per_month[year_index][mo_k] * SECS_PER_DAY;
    }

    ts += timeinfo->tm_mday * SECS_PER_DAY;
    ts += timeinfo->tm_hour * SECS_PER_HOUR;
    ts += timeinfo->tm_min * SECS_PER_MIN;
    ts += timeinfo->tm_sec;

    return ts;
}
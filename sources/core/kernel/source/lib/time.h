#ifndef LIB_TIME_H
#define LIB_TIME_H 1

#include <time.h>
#include <global/time.h>

#define TM_YEAR_BASE 1900

time_t mktime(struct tm* timeinfo);

#endif // LIB_TIME_H
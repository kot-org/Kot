#pragma once

#define CMOS_ADDR   0x70
#define CMOS_DATA   0x71

#define RTC_SECOND      0x0
#define RTC_MINUTE      0x2
#define RTC_HOUR        0x4
#define RTC_DOW         0x6 // day of week
#define RTC_DOM         0x7 // day of month
#define RTC_MONTH       0x8
#define RTC_YEAR        0x9
#define RTC_CENTURY     0x32

#define RTC_REGA    0xA

#include <kot/sys.h>
#include <kot/cstring.h>
#include <kot/uisd/srvs/time.h>
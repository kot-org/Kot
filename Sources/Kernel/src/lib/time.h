#pragma once
#include <kot/types.h>

struct Time{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t days;
    uint8_t months;
    uint64_t years;
}__attribute__((packed));
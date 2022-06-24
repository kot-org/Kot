#ifndef _MATH_H
#define _MATH_H 1

#include <kot/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

struct UPoint_t{
    uint64_t x;
    uint64_t y;
};

struct Point_t{
    int64_t x;
    int64_t y;
};

uint64_t DivideRoundUp(uint64_t value, uint64_t divider);

#if defined(__cplusplus)
}
#endif

#endif
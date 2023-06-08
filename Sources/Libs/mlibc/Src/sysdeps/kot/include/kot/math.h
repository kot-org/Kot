#include <stdint.h>

#ifndef KOT_MATH_H
#define KOT_MATH_H 1

#if defined(__cplusplus)
extern "C" {
#endif

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

typedef struct{
    uint64_t x;
    uint64_t y;
}kot_upoint_t;

typedef struct{
    int64_t x;
    int64_t y;
}kot_point_t;

int ipow(int base, int exp);

int8_t kot_sgn(int64_t x);

#if defined(__cplusplus)
} 
#endif

#endif
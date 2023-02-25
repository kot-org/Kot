#ifndef _MATH_H
#define _MATH_H 1

#include <kot/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define M_PI 3.14159265358979323846

typedef struct{
    uint64_t x;
    uint64_t y;
}upoint_t;

typedef struct{
    int64_t x;
    int64_t y;
}point_t;

uint64_t DivideRoundUp(uint64_t value, uint64_t divider);

int8_t sgn(int64_t value);
int64_t abs(int64_t value);
double fabs(double x);
int64_t min(int64_t a, int64_t b);
int64_t max(int64_t a, int64_t b);
int exponentInt(const int base, int n);
double pow(double x, double y);
double sqrt(double x);
double cos(double x);
double sin(double x);

#if defined(__cplusplus)
}
#endif

#endif
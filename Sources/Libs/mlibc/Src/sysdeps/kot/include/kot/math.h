#ifndef KOT_MATH_H
#define KOT_MATH_H 1

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

typedef struct{
    uint64_t x;
    uint64_t y;
}kot_upoint_t;

typedef struct{
    int64_t x;
    int64_t y;
}kot_point_t;

#endif
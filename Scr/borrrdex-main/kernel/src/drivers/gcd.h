#pragma once

#include "drivers/device.h"

typedef struct gcd_struct {
    device_t* device;
    int(*write)(struct gcd_struct*, const void*, int);
    int(*read)(struct gcd_struct*, void*, int);
} gcd_t;
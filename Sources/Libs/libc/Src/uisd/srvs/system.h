#ifndef _SRV_SYSTEM_H
#define _SRV_SYSTEM_H 1

#include <kot/types.h>

typedef struct srv_system_framebuffer_t{
    uint64_t address;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint64_t bpp;
};

#endif
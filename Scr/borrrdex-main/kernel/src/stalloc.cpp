#include "stalloc.h"
#include "Panic.h"

extern char _KernelEnd;

static uint64_t s_free_area_start;
uint64_t s_stalloced_total;

#define ALIGN_TO_4B(x) if((x) & 0x03) { \
    (x) += 4;\
    (x) &= ~0x03ULL;\
}

void stalloc_disable() {
    s_stalloced_total = s_free_area_start - ((uint64_t)&_KernelEnd);
    s_free_area_start = 0xffffffff;
}

void stalloc_init() {
    s_free_area_start = (uint64_t)&_KernelEnd;
    s_stalloced_total = 0;

    ALIGN_TO_4B(s_free_area_start);
}

uint64_t* stalloc(int bytes) {
    if(s_free_area_start == 0xffffffff) {
        Panic("Attempting to use stalloc after vm init");
    }

    if(s_free_area_start == 0) {
        Panic("Attempting to use stalloc without initialization");
    }

    if(bytes < 0) {
        Panic("Attempting to stalloc negative amount of bytes");
    }

    uint64_t res = s_free_area_start;
    s_free_area_start += bytes;
    ALIGN_TO_4B(s_free_area_start);

    return (uint64_t *)res;
}
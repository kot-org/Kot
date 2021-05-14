#include "fpu.h"

void FPUInit() {
    asm volatile("finit");
}
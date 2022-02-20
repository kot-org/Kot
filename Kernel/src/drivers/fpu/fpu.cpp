#include <drivers/fpu/fpu.h>

void FPUInit() {
    asm volatile("finit");
}
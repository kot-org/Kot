#pragma once
#include "../../lib/limits.h"
#include "../../lib/types.h"
#include "../../arch/x86-64/io/io.h"
#include "../../drivers/graphics/graphics.h"
#include "../../lib/stdio/cstr.h"

namespace PIT {
    const uint64_t BaseFrequency = 1193182; //evry once second

    void Sleepd(double second);
    void Sleep(uint64_t milliSeconds);

    void SetDivisor(uint16_t divisor);
    uint64_t GetFrequency();
    void SetFrequency(uint64_t frequency);
    void Tick();
}
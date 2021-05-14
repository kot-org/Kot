#pragma once

#include "math.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    void ps2_mouse_init();
    void ps2_mouse_process_packet();

    extern Point MousePosition;

#ifdef __cplusplus
}
#endif
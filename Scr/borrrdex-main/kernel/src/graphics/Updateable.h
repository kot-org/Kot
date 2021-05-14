#pragma once

#ifdef VIRTUAL_METHODS
#include "../io/rtc.h"

struct Updateable {
    virtual void tick(datetime_t* dt) { };
    virtual uint16_t get_update_ticks() const { return rtc_get_interrupt_frequency_hz(); };
};
#endif
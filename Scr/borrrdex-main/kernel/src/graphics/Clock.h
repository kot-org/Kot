#pragma once

#include "BasicRenderer.h"
#include "Updateable.h"
#include "arch/x86_64/io/rtc.h"

class Clock 
#ifdef VIRTUAL_METHODS
    : public Updateable
#endif
{
    public:
        Clock() : Clock(GlobalRenderer) { }
        Clock(BasicRenderer* renderer)
            :_renderer(renderer)
        {

        }

        #ifdef VIRTUAL_METHODS
    void tick(datetime_t* tm) override;
    uint16_t get_update_ticks() const override { return rtc_get_interrupt_frequency_hz() / 4; };
    #else
    void tick(datetime_t* tm);
    uint16_t get_update_ticks() const { return rtc_get_interrupt_frequency_hz() / 4; };
    #endif
    private:
        BasicRenderer* _renderer;
};
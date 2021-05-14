#pragma once

#include "../math.h"
#include "arch/x86_64/io/rtc.h"
#include "Updateable.h"
#include <stdint.h>



class Framebuffer;
class PSF1_FONT;

class BasicRenderer
#ifdef VIRTUAL_METHODS
    : public Updateable
#endif
{
public:
    BasicRenderer(Framebuffer* targetFrameBuffer, PSF1_FONT* font);

    void PutCharAt(char chr, unsigned xOff, unsigned yOff);
    void PutChar(char chr);
    void Printf(const char* str, ...);
    void PrintN(const char* str, uint64_t len);
    void Up();
    void Left();
    void Right();
    void Down();
    void Next();

    void SetColor(unsigned color) { _color = color; }
    void SetBackground(unsigned color) { _background = color; };
    void Clear();
    void ClearChar();
    
    Point CursorPosition {};
    unsigned Width() const;
    unsigned Height() const;
    
    #ifdef VIRTUAL_METHODS
    void tick(datetime_t* tm) override;
    uint16_t get_update_ticks() const override { return rtc_get_interrupt_frequency_hz() / 2; };
    #else
    void tick(datetime_t* tm);
    uint16_t get_update_ticks() const { return rtc_get_interrupt_frequency_hz() / 2; };
    #endif
private:
    Framebuffer* _targetFrameBuffer;
    PSF1_FONT* _psf1Font;
    unsigned _color {0xffffffff};
    unsigned _background {0x0};

    void ClearCurrent();
    void ClearCursor();
    void ShowCursor();
    bool HasChar();

    bool _cursorVisible {false};

};

extern BasicRenderer* GlobalRenderer;
#include "Clock.h"

const char* dow[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void print_padded(BasicRenderer* r, uint64_t val) {
    if(val < 10) {
        r->Printf("0");
    }

    r->Printf("%llu", val);
}

void Clock::tick(datetime_t* dt) {
    Point prev = _renderer->CursorPosition;
    _renderer->CursorPosition.x = _renderer->Width() - 8 * 23;
    _renderer->CursorPosition.y = 0;
    _renderer->Printf("%s %hu/%02hhu/%02hhu %02hhu:%02hhu:%02hhu",
        dow[dt->weekday - 1], dt->year, dt->month, dt->day, dt->hours, dt->minutes, dt->seconds);

    _renderer->CursorPosition = prev;
}
#include <kot/types.h>
#include <kot/atomic.h>

enum mousePortType{
    mousePortTypePS2        = 0x0,
    mousePortTypeUSB        = 0x1,
    mousePortTypeWireless   = 0x2,
    mousePortTypeUnknow     = 0xff,
};

enum mouseType{
    mouseTypeGeneric        = 0x0,
    mouseTypeScroll         = 0x1,
    mouseTypeScroll5Buttons = 0x2,    
    mouseTypeUnknow         = 0xff,
};

struct MouseData_t{
    enum mousePortType mousePortType;
    enum mouseType mouseType;

    uint64_t xAxisOffsetMax;
    uint64_t yAxisOffsetMax;

    uint64_t xAxisOffset;
    uint64_t yAxisOffset;

    kevent_t onMouseStateChanged;

    bool IsInitialized;
}__attribute__((packed));
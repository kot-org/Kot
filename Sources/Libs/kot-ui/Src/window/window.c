#include <kot-ui/window.h>

#include <kot/sys.h>

void CreateTitleBar(framebuffer_t* fb) {
    fillRect(fb, 0, 30, fb->width, 30, 0x323232);

    Printlog("ok");
}
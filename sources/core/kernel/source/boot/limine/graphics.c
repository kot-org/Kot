#include <impl/graphics.h>
#include <impl/initrd.h>
#include <global/console.h>

static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

const char* term_wallpaper_path = "/system/kterm/term_wallpaper.bmp";

void graphics_init(void) {
    if(framebuffer_request.response->framebuffer_count > 0) {
        struct limine_framebuffer* framebuffer = framebuffer_request.response->framebuffers[0];

        console_init(framebuffer->address, framebuffer->width, framebuffer->height, framebuffer->pitch, framebuffer->bpp);
    }
}
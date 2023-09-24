#include <boot/limine.h>
#include <impl/initrd.h>
#include <impl/graphics.h>
#include <global/console.h>

graphics_boot_fb_t boot_fb;

static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

const char* term_wallpaper_path = "/system/kterm/term_wallpaper.bmp";

void graphics_init(void) {
    if(framebuffer_request.response->framebuffer_count > 0) {
        struct limine_framebuffer* framebuffer = framebuffer_request.response->framebuffers[0];

        boot_fb.base = framebuffer->address;
        boot_fb.size = framebuffer->pitch * framebuffer->height;
        boot_fb.width = framebuffer->width;
        boot_fb.height = framebuffer->height;
        boot_fb.pitch = framebuffer->pitch;
        boot_fb.bpp = framebuffer->bpp;
        boot_fb.btpp = framebuffer->bpp / 8;

        console_init(&boot_fb);
    }
}

graphics_boot_fb_t* graphics_get_boot_fb(void) {
    return &boot_fb;
}
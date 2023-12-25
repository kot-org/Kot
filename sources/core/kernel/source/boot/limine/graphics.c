#include <boot/limine.h>
#include <impl/initrd.h>
#include <impl/graphics.h>
#include <global/console.h>

static graphics_boot_fb_t boot_fb;
static graphics_ask_ownership_boot_fb_t last_owner_callback = NULL;

static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

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

        console_init();
    }
}


graphics_boot_fb_t* graphics_get_boot_fb(graphics_ask_ownership_boot_fb_t owner_callback) {
    if(last_owner_callback != NULL){
        if(last_owner_callback()){
            return NULL;
        }
    }

    last_owner_callback = owner_callback;

    return &boot_fb;
}
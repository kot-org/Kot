#include <boot/limine.h>

#include <impl/initrd.h>
#include <global/term.h>

static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

const char* term_wallpaper_path = "/term_wallpaper.bmp";

void graphics_init(void) {
    if(framebuffer_request.response->framebuffer_count) {
        struct limine_framebuffer* framebuffer = framebuffer_request.response->framebuffers[0];
        void* file_ptr = initrd_get_file(term_wallpaper_path);
        void* file_base = NULL;
        size_t file_size = 0;
        if(file_ptr){
            file_base = initrd_get_file_base(file_ptr);
            file_size = initrd_get_file_size(file_ptr);
        }

        init_term(framebuffer->address, framebuffer->width, framebuffer->height, framebuffer->pitch, file_base, file_size);
    }
}
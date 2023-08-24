#include <lib/string.h>
#include <global/heap.h>
#include <global/term/term.h>
#include <global/term/vgafont.h>

static struct term_t terminal;

void* alloc_mem(size_t size) {
    return calloc(size, sizeof(uint8_t));
}

void free_mem(void *ptr, size_t size) {
    free(ptr);
}

void callback(struct term_t* term, uint64_t type, uint64_t gp0, uint64_t gp1, uint64_t gp2) {
    
}

void init_term(void* fb_base, uint64_t fb_width, uint64_t fb_height, uint64_t fb_pitch, void* image_base, size_t image_size) {
    struct framebuffer_t frm = {
        (uintptr_t)fb_base,
        fb_width,
        fb_height,
        fb_pitch
    };

    struct font_t font = {
        (uintptr_t)&vgafont,
        8,
        16,
        1,
        1,
        1 
    };

    struct style_t style = {
        DEFAULT_ANSI_COLOURS,
        DEFAULT_ANSI_BRIGHT_COLOURS,
        DEFAULT_BACKGROUND,
        DEFAULT_FOREGROUND,
        DEFAULT_MARGIN,
        DEFAULT_MARGIN_GRADIENT
    };

    struct background_t back;
    struct image_t image;

    if(image_base && image_size){
        image_open(&image, (uint64_t)image_base, image_size);
        back = (struct background_t){
            &image,
            STRETCHED,
            DEFAULT_BACKGROUND 
        };
    }else{
        back = (struct background_t){
            NULL,
            STRETCHED,
            DEFAULT_BACKGROUND 
        };
    }


    term_init(&terminal, callback, false, TERM_TABSIZE);

    term_vbe(&terminal, frm, font, style, back);
}

void put_char_terminal(char c) {
    term_putchar(&terminal, c);
    term_double_buffer_flush(&terminal);
}

void write_terminal(const char* text, size_t size) {
    term_write(&terminal, text, size);
}

void print_terminal(const char* text) {
    write_terminal(text, strlen(text));
}
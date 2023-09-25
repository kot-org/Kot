#include <global/console.h>

#include "vgafont.h"
#include "ansi.h"

#include <lib/lock.h>
#include <lib/bitmap.h>
#include <lib/memory.h>
#include <lib/assert.h>
#include <impl/serial.h>
#include <lib/string.h>

static bool use_boot_fb = false;
static spinlock_t boot_fb_lock = {};

static uint32_t bg_color;
static uint32_t fg_color;

static uint32_t* fb_base;
static uint16_t fb_width, fb_height;
static uint16_t fb_pitch;
static uint8_t fb_bpp;
static uint8_t fb_btpp;
static size_t fb_size;


// char position
static uint16_t cx_index;
static uint16_t cy_index;

static uint16_t cx_max_index;
static uint16_t cy_max_index;

void console_set_bg_color(uint32_t bg) {
    bg_color = bg;
}
void console_set_fg_color(uint32_t fg) {
    fg_color = fg;
}

static void console_putpixel(uint16_t x, uint16_t y, uint32_t color) {
    fb_base[y*fb_width+x] = color;
}

static void console_scroll_line(void){
    cy_index--;
    
    size_t line_size = (size_t)fb_pitch * (size_t)VGAFONT_HEIGHT;
    void* fb_base_copy = (void*)((uintptr_t)fb_base + (uintptr_t)line_size);

    size_t size_to_copy = fb_size - line_size;
    memcpy(fb_base, fb_base_copy, size_to_copy);
    
    void* fb_base_to_clear = (void*)((uintptr_t)fb_base + (uintptr_t)size_to_copy);
    memset32(fb_base_to_clear, DEFAULT_BG_COLOR, size_to_copy / sizeof(uint32_t));
}

static void console_new_line(void){
    cx_index = 0;
    cy_index++;
}

static int boot_fb_callback(void){
    console_print("Warning : console will no more use the framebuffer for this session\n");
    spinlock_acquire(&boot_fb_lock);
    use_boot_fb = false;
    spinlock_release(&boot_fb_lock);
    return 0;
}

void console_init(void) {
    console_set_bg_color(DEFAULT_BG_COLOR);
    console_set_fg_color(DEFAULT_FG_COLOR);

    graphics_boot_fb_t* boot_fb = graphics_get_boot_fb(&boot_fb_callback);

    if(boot_fb == NULL){
        use_boot_fb = false;
    }else{
        if(boot_fb->bpp != 32){
            use_boot_fb = false;
        }else{
            use_boot_fb = true;

            fb_base = boot_fb->base;
            fb_width = boot_fb->width;
            fb_height = boot_fb->height;
            fb_pitch = boot_fb->pitch;
            fb_bpp = boot_fb->bpp;
            fb_btpp = boot_fb->btpp;
            fb_size = boot_fb->size;
        }
        cx_index = 0;
        cy_index = 0;

        cx_max_index = fb_width / VGAFONT_WIDTH;
        cy_max_index = fb_height / VGAFONT_HEIGHT;

        memset32(fb_base, DEFAULT_BG_COLOR, fb_size / sizeof(uint32_t));
    }
}

void console_putchar(char c) {
    spinlock_acquire(&boot_fb_lock);

    if(!use_boot_fb){
        spinlock_release(&boot_fb_lock);
        return;
    }

    uint8_t* glyph = &vgafont[c*((VGAFONT_WIDTH*VGAFONT_HEIGHT)/8)];

    // char pixel-position
    uint16_t cx_ppos = cx_index * VGAFONT_WIDTH;
    if((cx_ppos + VGAFONT_WIDTH) > fb_width) {
        console_new_line();
    }

    uint16_t cy_ppos = cy_index * VGAFONT_HEIGHT;
    if((cy_ppos + VGAFONT_HEIGHT) > fb_height) {
        console_scroll_line();
    }

    if(c == '\n') {
        console_new_line();
        spinlock_release(&boot_fb_lock);
        return;
    }
    if(c == '\r') {
        cx_index = 0;
        spinlock_release(&boot_fb_lock);
        return;
    }

    for(uint16_t y = 0; y < VGAFONT_HEIGHT; y++) {
        for(uint16_t x = 0; x < VGAFONT_WIDTH; x++) {
            
            if(BIT_GET(glyph[y], VGAFONT_WIDTH-x) == BITSET) {
                console_putpixel(cx_ppos+x, cy_ppos, fg_color); 
            } else {
                console_putpixel(cx_ppos+x, cy_ppos, bg_color); 
            }
        }
        cy_ppos++;
    }
    cx_index++;

    spinlock_release(&boot_fb_lock);
}

void console_print(const char* str) {
    for(size_t i = 0; i < strlen(str); i++) {
        if(str[i] == ANSI_CONTROL || str[i] == '\033') {
            size_t next_char_position = (size_t)ansi_read(str+i) + i;
            for(; i < next_char_position; i++){
                serial_write(str[i]); // use the ANSI code for serial
            }
        }

        console_putchar(str[i]);
        serial_write(str[i]);

        if(str[i] == '\n'){
            console_putchar('\r');
            serial_write('\r');   
        }
    }
}
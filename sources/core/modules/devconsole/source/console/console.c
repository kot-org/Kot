#include "console.h"
#include "ansi.h"

#include <kernel.h>
#include <lib/log.h>
#include <lib/lock.h>
#include <lib/string.h>
#include <lib/printf.h>
#include <lib/bitmap.h>
#include <lib/memory.h>
#include <lib/assert.h>
#include <impl/serial.h>
#include <impl/initrd.h>

#define DEFAULT_CUT_COLOR_DEVCONSOLE 0x00995A

#define CONSOLE_VERSION "1.0"

#define FONT_WIDTH 8
#define FONT_HEIGHT 16

#define TAB_SIZE 8

static bool use_boot_fb = false;
static spinlock_t boot_fb_lock = SPINLOCK_INIT;

static uint32_t bg_color;
static uint32_t fg_color;

static uint32_t* fb_foreground_base;
static uint32_t* fb_background_base;
static uint16_t fb_width, fb_height;
static uint16_t fb_pitch;
static uint8_t fb_bpp;
static uint8_t fb_btpp;
static size_t fb_size;

static uint16_t line_count = 0;

static uint16_t cx_max_index;
static uint16_t cy_max_index;

static uint16_t cy_info_index;

static uint8_t* font_buffer;
static size_t font_size;

// cursor position
static uint16_t cx_index;
static uint16_t cy_index;

static uint16_t last_cx_index = 0;
static uint16_t last_cy_index = 0;

static uint32_t cursor_color = DEFAULT_FG_COLOR;


void key_handler(uint64_t scancode, uint16_t translated_key, bool is_pressed);

void devconsole_set_bg_color(uint32_t bg) {
    bg_color = bg;
}
void devconsole_set_fg_color(uint32_t fg) {
    fg_color = fg;
}

static void devconsole_putpixel(uint16_t x, uint16_t y, uint32_t color) {
    fb_foreground_base[y*fb_width+x] = color;
    fb_background_base[y*fb_width+x] = color;
}

static void devconsole_setchar(uint16_t cx_ppos, uint16_t cy_ppos, char c){
    uint8_t* glyph = &font_buffer[c*((FONT_WIDTH*FONT_HEIGHT)/8)];
    
    for(uint16_t y = 0; y < FONT_HEIGHT; y++) {
        for(uint16_t x = 0; x < FONT_WIDTH; x++) {
            
            if(BIT_GET(glyph[y], FONT_WIDTH-x) == BITSET) {
                devconsole_putpixel(cx_ppos+x, cy_ppos, fg_color); 
            }
        }
        cy_ppos++;
    }
}

static void devconsole_clearchar(uint16_t cx_ppos, uint16_t cy_ppos){    
    for(uint16_t y = 0; y < FONT_HEIGHT; y++) {
        for(uint16_t x = 0; x < FONT_WIDTH; x++) {
            devconsole_putpixel(cx_ppos+x, cy_ppos, bg_color); 
        }
        cy_ppos++;
    }
}

static void devconsole_printline(uint16_t x, uint16_t line, char* str){
    for(size_t i = 0; i < strlen(str); i++) {
        devconsole_setchar((x + i) * FONT_WIDTH, line * FONT_HEIGHT, str[i]);
    }
}

static void devconsole_new_line(void){
    line_count++;

    size_t line_size = (size_t)fb_pitch * (size_t)FONT_HEIGHT;
    size_t line_pixel_count = (size_t)fb_width * (size_t)FONT_HEIGHT;

    cx_index = 0;
    if(cy_index < cy_max_index){
        cy_index++;
    }else{
        void* fb_base_move = (void*)((uintptr_t)fb_foreground_base + (uintptr_t)line_size);
        size_t size_to_move = line_size * cy_max_index;
        memcpy(fb_foreground_base, fb_base_move, size_to_move);
        void* fb_base_to_clear_background = (void*)((uintptr_t)fb_background_base + (uintptr_t)line_size * cy_max_index);
        memset32(fb_base_to_clear_background , bg_color, line_pixel_count);
        void* fb_base_to_clear_foreground = (void*)((uintptr_t)fb_foreground_base + (uintptr_t)line_size * cy_max_index);
        memset32(fb_base_to_clear_foreground , bg_color, line_pixel_count);
    }

    void* fb_base_to_cut = (void*)((uintptr_t)fb_foreground_base + (uintptr_t)line_size * cy_info_index);
    memset32(fb_base_to_cut, DEFAULT_CUT_COLOR_DEVCONSOLE, line_pixel_count);

    char cut_buffer[cx_max_index];
    snprintf_((char*)&cut_buffer, cx_max_index, "Kernel version : %s | Console version : %s | Number of lines written : %d", KERNEL_VERSION, CONSOLE_VERSION, line_count);
    devconsole_printline(0, cy_info_index, cut_buffer);
}

static int boot_fb_callback(void){
    spinlock_acquire(&boot_fb_lock);
    use_boot_fb = false;
    spinlock_release(&boot_fb_lock);
    return 0;
}

void devconsole_request_fb(void){
    if(use_boot_fb){
        return;
    }

    /* get key handler */
    hid_handler->set_key_handler(&key_handler);

    use_boot_fb = true;

    graphics_boot_fb_t* boot_fb = graphics_get_boot_fb(&boot_fb_callback);

    if(boot_fb == NULL){
        use_boot_fb = false;
    }else{
        if(boot_fb->bpp != 32){
            use_boot_fb = false;
        }else{
            use_boot_fb = true;

            fb_foreground_base = boot_fb->base;
            fb_background_base = malloc(boot_fb->size);
            fb_width = boot_fb->width;
            fb_height = boot_fb->height;
            fb_pitch = boot_fb->pitch;
            fb_bpp = boot_fb->bpp;
            fb_btpp = boot_fb->btpp;
            fb_size = boot_fb->size;
        }
        cx_index = 0;
        cy_index = 0;

        cx_max_index = fb_width / FONT_WIDTH;
        cy_info_index = (fb_height / FONT_HEIGHT) - 1;
        cy_max_index = cy_info_index - 1;

        memset32(fb_background_base, bg_color, fb_size / sizeof(uint32_t));
        memset32(fb_foreground_base, bg_color, fb_size / sizeof(uint32_t));
    }
}

// CURSOR

void cursor_draw(void) {
    // draw new cursor
    for(uint8_t i = 0; i < FONT_HEIGHT; i++) {
        devconsole_putpixel(cx_index*FONT_WIDTH, cy_index*FONT_HEIGHT+i, cursor_color);
    }

    last_cx_index = cx_index;
    last_cy_index = cy_index;
}

void cursor_remove(void) {
    for(uint8_t i = 0; i < FONT_HEIGHT; i++) {
        devconsole_putpixel(cx_index*FONT_WIDTH, cy_index*FONT_HEIGHT+i, bg_color);
    }
}

void cursor_removelast(void) {
    for(uint8_t i = 0; i < FONT_HEIGHT; i++) {
        devconsole_putpixel(last_cx_index*FONT_WIDTH, last_cy_index*FONT_HEIGHT+i, bg_color);
    }
}

void cursor_update(void) {
    cursor_removelast();
    cursor_draw();
}

// END CURSOR

void devconsole_init(void) { 
    void* font_file = initrd_get_file("/system/console/fonts/vga.bin");
    font_size = initrd_get_file_size(font_file);
    font_buffer = initrd_get_file_base(font_file);

    devconsole_set_bg_color(DEFAULT_BG_COLOR);
    devconsole_set_fg_color(DEFAULT_FG_COLOR);
}

void devconsole_putchar(char c) {
    spinlock_acquire(&boot_fb_lock);

    if(!use_boot_fb){
        spinlock_release(&boot_fb_lock);
        return;
    }

    // char pixel-position
    uint16_t cx_ppos = cx_index * FONT_WIDTH;
    if((cx_ppos + FONT_WIDTH) > fb_width) {
        devconsole_new_line();
    }

    uint16_t cy_ppos = cy_index * FONT_HEIGHT;

    if(c == '\n') {
        devconsole_new_line();
        spinlock_release(&boot_fb_lock);
        return;
    }
    if(c == '\r') {
        cx_index = 0;
        spinlock_release(&boot_fb_lock);
        return;
    }

    if(c == 9){ // tab
        if(cx_index % TAB_SIZE){
            cx_index -= cx_index % TAB_SIZE;
        }

        cx_index += TAB_SIZE;
        spinlock_release(&boot_fb_lock);
        return;        
    }

    devconsole_setchar(cx_ppos, cy_ppos, c);

    cx_index++;

    spinlock_release(&boot_fb_lock);
}

void devconsole_delchar(void){
    spinlock_acquire(&boot_fb_lock);
    if(cx_index != 0){
        cx_index--;
    }else{
        cx_index = cx_max_index - 1;
        if(cy_index == 0){
            cy_index = cy_max_index - 1;
        }else{
            cy_index = (cy_index - 1) % cy_max_index;
        }
    }
    uint16_t cx_ppos = cx_index * FONT_WIDTH;
    uint16_t cy_ppos = cy_index * FONT_HEIGHT;

    devconsole_clearchar(cx_ppos, cy_ppos);

    spinlock_release(&boot_fb_lock);
}

void devconsole_print(const char* str, size_t size) {
    for(size_t i = 0; i < size; i++) {
        if(str[i] == '\n'){
            devconsole_putchar('\r');
            serial_write('\r');   
        }

        if(str[i] == '\e' || str[i] == '\033') {
            i = (size_t)ansi_read(str+i) + i;
        }

        devconsole_putchar(str[i]);
        serial_write(str[i]);
    }
}

void devconsole_update_screen(void){
    memcpy((void*)fb_foreground_base, (void*)fb_background_base, fb_size);
}
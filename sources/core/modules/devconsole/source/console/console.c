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

#define CONSOLE_VERSION "1.2"

#define FONT_WIDTH 8
#define FONT_HEIGHT 16

#define TAB_SIZE 8

#define ESCAPE_BUFFER_MAX_SIZE 128

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

static uint16_t cx_max_index;
static uint16_t cy_max_index;

static uint8_t* font_buffer;
static size_t font_size;

// cursor position
static uint16_t cx_index;
static uint16_t cy_index;

static uint16_t last_cx_index = 0;
static uint16_t last_cy_index = 0;

static uint32_t cursor_color = DEFAULT_FG_COLOR;

static char espacebuffer[ESCAPE_BUFFER_MAX_SIZE];
static size_t espacebuffersize = 0;


enum devconsole_parse_state {
    devconsole_parse_state_normal,
    devconsole_parse_state_escape,
    devconsole_parse_state_control,
    devconsole_parse_state_graphics,
};

static enum devconsole_parse_state parse_state = devconsole_parse_state_normal;

static uint32_t fg_colors[10] = {0x000000, 0xCD3131, 0x0DBC79, 0xE5E510, 0x2472C8, 0xBC3FBC, 0x11A8CD, 0xCFCFCF, DEFAULT_FG_COLOR};
static uint32_t fg_bright_colors[9] = {0x666666, 0xF14C4C, 0x23D18B, 0xF5F543, 0x3B8EEA, 0xD670D6, 0x29B8DB, 0xFFFFFF};
static uint32_t bg_colors[10] = {0x000000, 0xCD3131, 0x0DBC79, 0xE5E510, 0x2472C8, 0xBC3FBC, 0x11A8CD, 0xCFCFCF, DEFAULT_BG_COLOR};
static uint32_t bg_bright_colors[9] = {0x666666, 0xF14C4C, 0x23D18B, 0xF5F543, 0x3B8EEA, 0xD670D6, 0x29B8DB, 0xFFFFFF};

static bool refresh_display = false;


void key_handler(uint64_t scancode, uint16_t translated_key, bool is_pressed);

void devconsole_set_bg_color(uint32_t bg) {
    bg_color = bg;
}

uint32_t devconsole_get_bg_color(void) {
    return bg_color;
}
void devconsole_set_fg_color(uint32_t fg) {
    fg_color = fg;
}

uint32_t devconsole_get_fg_color(void) {
    return fg_color;
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
            }else{
                devconsole_putpixel(cx_ppos+x, cy_ppos, bg_color); 
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
    size_t line_size = (size_t)fb_pitch * (size_t)FONT_HEIGHT;
    size_t line_pixel_count = (size_t)fb_width * (size_t)FONT_HEIGHT;

    if(cy_index < cy_max_index){
        int count = cx_max_index - cx_index + 1;
        for(int i = 0; i < count; i++){
            devconsole_clearchar((cx_index + i) * FONT_WIDTH, cy_index * FONT_HEIGHT);
        }
        cy_index++;
    }else{
        void* fb_base_move = (void*)((uintptr_t)fb_foreground_base + (uintptr_t)line_size);
        size_t size_to_move = line_size * cy_max_index;
        memcpy(fb_foreground_base, fb_base_move, size_to_move);
        void* fb_base_to_clear_background = (void*)((uintptr_t)fb_background_base + (uintptr_t)line_size * cy_max_index);
        memset32(fb_base_to_clear_background, bg_color, line_pixel_count);
        void* fb_base_to_clear_foreground = (void*)((uintptr_t)fb_foreground_base + (uintptr_t)line_size * cy_max_index);
        memset32(fb_base_to_clear_foreground, bg_color, line_pixel_count);
    }
    cx_index = 0;
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

        cx_max_index = fb_width / FONT_WIDTH - 1;
        cy_max_index = fb_height / FONT_HEIGHT - 1;

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

void cursor_remove_last(void) {
    for(uint8_t i = 0; i < FONT_HEIGHT; i++) {
        devconsole_putpixel(last_cx_index*FONT_WIDTH, last_cy_index*FONT_HEIGHT+i, bg_color);
    }
}

void cursor_update(void) {
    cursor_remove_last();
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

    if(c == '\t'){ // tab
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
        if(cy_index != 0 && cx_index != 0){
            cx_index = cx_max_index - 1;
            cy_index = (cy_index - 1) % cy_max_index;
        }
    }
    uint16_t cx_ppos = cx_index * FONT_WIDTH;
    uint16_t cy_ppos = cy_index * FONT_HEIGHT;

    devconsole_clearchar(cx_ppos, cy_ppos);

    spinlock_release(&boot_fb_lock);
}

bool devconsole_isprintable(uint16_t c){
    return (c >= 0x21 && c <= 0x7E) || c == ' ';
}

bool devconsole_aplha(uint16_t c){
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

void dev_console_appendescapebuffer(char c){
    if(espacebuffersize + 1 < ESCAPE_BUFFER_MAX_SIZE){
        espacebuffer[espacebuffersize] = c;
        espacebuffersize++;
        espacebuffer[espacebuffersize] = '\0';
    }
}

void dev_console_clearescapebuffer(void){
    espacebuffersize = 0;
    espacebuffer[espacebuffersize] = '\0';
}

char* dev_console_get_espacebuffer(void){
    return espacebuffer;
}

void dev_console_clear(int mode){
    dev_check_display_update();

    switch (mode){
        case 0:{
            {
                size_t size_to_clear = (size_t)FONT_WIDTH * (cx_max_index - cx_index) * (size_t)FONT_HEIGHT;
                void* background_base_to_clear = (void*)((uintptr_t)fb_background_base + cx_index * FONT_HEIGHT * FONT_HEIGHT);
                memset32(background_base_to_clear, bg_color, size_to_clear);
            }
            {
                size_t size_to_clear = (size_t)fb_width * (size_t)FONT_HEIGHT * (size_t)(cy_max_index - (cy_index + 1));
                void* background_base_to_clear = (void*)((uintptr_t)fb_background_base + (cy_index + 1) * FONT_HEIGHT * fb_pitch);
                memset32(background_base_to_clear, bg_color, size_to_clear);
            }
            break;
        }
        case 1:
        case 2:{
            size_t size_to_clear = (size_t)fb_width * (size_t)FONT_HEIGHT * (size_t)cy_max_index;
            memset32(fb_background_base, bg_color, size_to_clear);
            break;
        }
    }
    refresh_display = true;
}

void devconsole_parsechar(char c){
    //serial_write(c);
    if(parse_state == devconsole_parse_state_normal){
        if(devconsole_isprintable(c)){
            devconsole_putchar(c);
        }else if(c == '\n'){
            devconsole_putchar('\n');
            devconsole_putchar('\r');
        }else if(c == '\t'){
            devconsole_putchar('\t');
        }else if(c == '\e'){
            dev_console_clearescapebuffer();
            parse_state = devconsole_parse_state_escape;
        }else if(c == '\b'){

        }
    }else if(parse_state == devconsole_parse_state_escape){
        if(c == ANSI_CONTROL){
            parse_state = devconsole_parse_state_control;
        }else{
            parse_state = devconsole_parse_state_normal;
        }
    }else if(parse_state == devconsole_parse_state_control){
        if(!devconsole_aplha(c)){
            dev_console_appendescapebuffer(c);
        }else if(c == ANSI_GRAPHICS){
            parse_state = devconsole_parse_state_graphics;
        }else if(c == ANSI_CURSOR_POSITION){
            cx_index = 0;
            cy_index = 0;
            char* data = dev_console_get_espacebuffer();
            char* separator = strchr(data, ANSI_SEPARATOR);
            if(separator != NULL){
                *separator = '\0';
                cy_index = MIN(MAX(atoi(data), cy_index), cy_max_index);
                cx_index = MIN(MAX(atoi(separator + sizeof(ANSI_SEPARATOR)), cx_index), cx_max_index);
            }
            parse_state = devconsole_parse_state_normal; 
        }else if(c == ANSI_SCROLL_DOWN){
            int amount = 1;
            char* data = dev_console_get_espacebuffer();
            if(strlen(data)){
                amount = atoi(data);
            }
            for(int i = 0; i < amount; i++){
                devconsole_new_line();
            }
            parse_state = devconsole_parse_state_normal; 
        }else if(c == ANSI_CURSOR_DOWN){
            int amount = 1;
            char* data = dev_console_get_espacebuffer();
            if(strlen(data)){
                amount = atoi(data);
            }
            cy_index = MIN(cy_index + amount, cy_max_index);
            parse_state = devconsole_parse_state_normal; 
        }else if(c == ANSI_CURSOR_FORWARD){
            int amount = 1;
            char* data = dev_console_get_espacebuffer();
            if(strlen(data)){
                amount = atoi(data);
            }
            cx_index = MIN(cx_index + amount, cx_max_index);
            parse_state = devconsole_parse_state_normal; 
        }else if(c == ANSI_ERASE_IN_DISPLAY){
            int mode = atoi(dev_console_get_espacebuffer());
            dev_console_clear(mode);
            parse_state = devconsole_parse_state_normal; 
        }
    }
    
    if(parse_state == devconsole_parse_state_graphics){
        char* data = dev_console_get_espacebuffer();
        char* separator = strchr(data, ANSI_SEPARATOR);
        if(separator != NULL){
            data = separator + sizeof(ANSI_SEPARATOR);
        }
        int data_code = atoi(data);

        if(data_code == ANSI_GRAPHICS_RESET){
            devconsole_set_bg_color(DEFAULT_BG_COLOR);
            devconsole_set_fg_color(DEFAULT_FG_COLOR);
        }else if(data_code >= ANSI_GRAPHICS_FG_BLACK && data_code <= ANSI_GRAPHICS_FG_DEFAULT){
            // foreground colors
            devconsole_set_fg_color(fg_colors[data_code - ANSI_GRAPHICS_FG_BLACK]);
        }else if(data_code >= ANSI_GRAPHICS_FG_BLACK_BRIGHT && data_code <= ANSI_GRAPHICS_FG_WHITE_BRIGHT){
            // foreground colors bright
            devconsole_set_fg_color(fg_bright_colors[data_code - ANSI_GRAPHICS_FG_BLACK_BRIGHT]);
        }else if(data_code >= ANSI_GRAPHICS_BG_BLACK && data_code <= ANSI_GRAPHICS_BG_DEFAULT){
            // background colors
            devconsole_set_bg_color(bg_colors[data_code - ANSI_GRAPHICS_BG_BLACK]);

        }else if(data_code >= ANSI_GRAPHICS_BG_BLACK_BRIGHT && data_code <= ANSI_GRAPHICS_BG_WHITE_BRIGHT){
            // background colors bright
            devconsole_set_bg_color(bg_bright_colors[data_code - ANSI_GRAPHICS_BG_BLACK_BRIGHT]);
        }
        parse_state = devconsole_parse_state_normal; 
    }
}

void devconsole_print(const char* str, size_t size) {
    uint16_t last_cy_index = cy_index;

    for(size_t i = 0; i < size; i++) {
        devconsole_parsechar(str[i]);
    }

    if(last_cy_index != cy_index){
        dev_check_display_update();
    }
}

void dev_check_display_update(void){
    if(refresh_display){
        devconsole_update_display();
        refresh_display = false;
    }
}

void devconsole_update_display(void){
    memcpy(fb_foreground_base, fb_background_base, fb_size);
}
#include <math.h>
#include <fcntl.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>

#include <stdbool.h>

#include <linux/fb.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#include <kot-graphics/font.h>
#include <kot-graphics/utils.h>
#include <kot-graphics/image.h>

#define TEXT_WIDTH          (500)
#define TEXT_MARGIN         (100)
#define TITLE_SIZE          (36)
#define TEXT_SIZE           (27)
#define SIGNATURE_SIZE      (24)
#define TEXT_START          (TEXT_MARGIN / 2)
#define TEXT_COLOR          (0xffffff)
#define LINK_COLOR          (0x33ddff)
#define TEXT_BACKGROUND     (0x222222)
#define TEXT_BOX_WIDTH      (TEXT_WIDTH + TEXT_MARGIN)
#define SLIDE_TIME          (5000)
#define PROGRESSBAR_HEIGHT  (5)
#define PROGRESSBAR_COLOR   (0xffffff)
#define INFO_SIZE           (16)

kfont_t font;

int fb_fd = -1;
kframebuffer_t fb;
struct fb_fix_screeninfo fix_screeninfo;
struct fb_var_screeninfo var_screeninfo;

uint64_t get_ticks_ms(){
    struct timeval tv; 
    gettimeofday(&tv, NULL);
    uint64_t milliseconds = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return milliseconds;
}

void draw_frame(){
    write(fb_fd, fb.buffer, fb.size);
}

int get_key(int* pressed, uint64_t* key){
    int64_t buffer;
    if(read(fb_fd, &buffer, 1) > 0){
        if(buffer & ((uint64_t)1 << 63)){
            *pressed = true;
        }else{
            *pressed = false;
        }
        *key = buffer & ~((uint64_t)1 << 63);
        return 1;
    }
    return 0;
}

int wait_for_the_next_slide(){
    int ret = 0;
    uint64_t start_tick = get_ticks_ms();
    uint64_t current_tick = start_tick; 
    uint64_t tick_to_stop = start_tick + SLIDE_TIME;

    set_pen_size(font, INFO_SIZE);
    write_paragraph(font, -1, fb.height - INFO_SIZE - (PROGRESSBAR_HEIGHT * 4) - 10, fmin(TEXT_WIDTH, fb.width), PARAGRAPH_CENTER, "Quit : <esc> | Next : <enter> | Pause : <p>\n");

    static bool is_enter_pressed = false;
    static bool is_pause_pressed = false;
    bool is_paused = false;
    uint64_t progress_size = 0;

    while(current_tick < tick_to_stop || is_paused){
        if(!is_paused){
            current_tick = get_ticks_ms();
            progress_size = ((current_tick - start_tick) * TEXT_BOX_WIDTH) / SLIDE_TIME;
            draw_rectangle(&fb, 0, fb.height - PROGRESSBAR_HEIGHT, progress_size, PROGRESSBAR_HEIGHT, PROGRESSBAR_COLOR);
            draw_frame();
        }

        int pressed;
        uint64_t key;
        if(get_key(&pressed, &key)){
            if(pressed && key == 28){
                if(!is_enter_pressed){
                    is_enter_pressed = true;
                    is_paused = false;
                    break;
                }
            }
            if(!pressed && key == 28){
                is_enter_pressed = false;
            }

            if(pressed && key == 1){
                ret = 1;
                break;
            }
            
            if(pressed && key == 25){
                if(!is_pause_pressed){
                    is_pause_pressed = true;
                    if(!is_paused){
                        is_paused = true;
                        draw_rectangle(&fb, 0, fb.height - PROGRESSBAR_HEIGHT, progress_size, PROGRESSBAR_HEIGHT, TEXT_BACKGROUND);
                        draw_frame();
                    }else{
                        start_tick = get_ticks_ms();
                        current_tick = start_tick; 
                        tick_to_stop = start_tick + SLIDE_TIME;
                        is_paused = false;
                    }
                }
            }
            if(!pressed && key == 25){
                is_pause_pressed = false;
            }
        }
    }

    return ret;
}

int main(int argc, char* argv[]){
    fb_fd = open("/dev/fb0", O_RDWR);

    printf("Loading welcome app...\n");

    assert(fb_fd >= 0);


    assert(ioctl(fb_fd, FBIOGET_FSCREENINFO, &fix_screeninfo) == 0);
    assert(ioctl(fb_fd, FBIOGET_VSCREENINFO, &var_screeninfo) == 0);

    if(
        fix_screeninfo.visual == FB_VISUAL_TRUECOLOR && 
        var_screeninfo.bits_per_pixel == 32 &&
        var_screeninfo.red.length == 8 && var_screeninfo.red.msb_right == 0 &&
        var_screeninfo.green.length == 8 && var_screeninfo.green.msb_right == 0 &&
        var_screeninfo.blue.length == 8 && var_screeninfo.blue.msb_right == 0 
    ){
        fb.bpp = var_screeninfo.bits_per_pixel;
        fb.btpp = fb.bpp / 8;
        fb.size = var_screeninfo.xres_virtual * var_screeninfo.yres_virtual * fb.btpp;
        fb.width = var_screeninfo.xres_virtual;
        fb.pitch = fb.width * fb.btpp;
        fb.height = var_screeninfo.yres_virtual;
        fb.buffer = malloc(fb.size);
        memset(fb.buffer, 0, fb.size);
    }else{
        perror("'/dev/fb0' : format not supported\n");
        return EXIT_FAILURE;
    }

    raw_image_t* wallpaper0 = load_jpeg_image_file("/usr/bin/res/welcome/welcome0.jpg");
    raw_image_t* wallpaper0_resized = resize_image(wallpaper0, 0, fb.height, true);
    free_raw_image(wallpaper0);

    raw_image_t* wallpaper1 = load_jpeg_image_file("/usr/bin/res/welcome/welcome1.jpg");
    raw_image_t* wallpaper1_resized = resize_image(wallpaper1, 0, fb.height, true);
    free_raw_image(wallpaper1);

    raw_image_t* wallpaper2 = load_jpeg_image_file("/usr/bin/res/welcome/welcome2.jpg");
    raw_image_t* wallpaper2_resized = resize_image(wallpaper2, 0, fb.height, true);
    free_raw_image(wallpaper2);

    raw_image_t* wallpaper3 = load_jpeg_image_file("/usr/bin/res/welcome/welcome3.jpg");
    raw_image_t* wallpaper3_resized = resize_image(wallpaper3, 0, fb.height, true);
    free_raw_image(wallpaper3);

    FILE* font_file = fopen("/usr/bin/res/welcome/welcome.ttf", "rb");

    if(font_file == NULL){
        perror("error loading font\n");
        return EXIT_FAILURE;
    }

    fseek(font_file, 0, SEEK_END);
    size_t font_file_size = ftell(font_file);
    fseek(font_file, 0, SEEK_SET);

    void* font_data = malloc(font_file_size);
    fread(font_data, font_file_size, 1, font_file);

    font = load_font(font_data, font_file_size);

    fclose(font_file);

    while(true){
        draw_image(&fb, wallpaper0_resized, TEXT_BOX_WIDTH, 0, fb.width - TEXT_BOX_WIDTH, fb.height);
        draw_rectangle(&fb, 0, 0, fmin(TEXT_BOX_WIDTH, fb.width), fb.height, TEXT_BACKGROUND);

        load_pen(font, &fb, TEXT_START, 0, TITLE_SIZE, 0, TEXT_COLOR);
        write_paragraph(font, -1, -1, fmin(TEXT_WIDTH, fb.width), PARAGRAPH_CENTER, "Welcome to Kot\n");

        set_pen_size(font, TEXT_SIZE);
        write_paragraph(font, -1, -1, fmin(TEXT_WIDTH, fb.width), PARAGRAPH_JUSTIFY, "\"Kot is nothing more than an Operating System running on x86-64. And you already know that, but we're making to turn Kot more than just something.\"\n");
        
        set_pen_size(font, SIGNATURE_SIZE);
        write_paragraph(font, -1, -1, fmin(TEXT_WIDTH, fb.width), PARAGRAPH_RIGHT, "Kot team\n");
        draw_frame();

        if(wait_for_the_next_slide()){
            break;
        }

        draw_image(&fb, wallpaper1_resized, TEXT_BOX_WIDTH, 0, fb.width - TEXT_BOX_WIDTH, fb.height);
        draw_rectangle(&fb, 0, 0, fmin(TEXT_BOX_WIDTH, fb.width), fb.height, TEXT_BACKGROUND);

        load_pen(font, &fb, TEXT_START, 0, TITLE_SIZE, 0, TEXT_COLOR);
        write_paragraph(font, -1, -1, fmin(TEXT_WIDTH, fb.width), PARAGRAPH_CENTER, "Credits :\n");

        set_pen_size(font, TEXT_SIZE);
        write_paragraph(font, -1, -1, fmin(TEXT_WIDTH, fb.width), PARAGRAPH_JUSTIFY, "Without the contributions of the following individuals (GitHub pseudonyms, listed alphabetically), Kot would not have existed:");
        write_paragraph(font, -1, -1, fmin(TEXT_WIDTH, fb.width), PARAGRAPH_LEFT, "- 0xS3B");
        write_paragraph(font, -1, -1, fmin(TEXT_WIDTH, fb.width), PARAGRAPH_LEFT, "- konect-V");
        write_paragraph(font, -1, -1, fmin(TEXT_WIDTH, fb.width), PARAGRAPH_LEFT, "- Moldytzu");
        write_paragraph(font, -1, -1, fmin(TEXT_WIDTH, fb.width), PARAGRAPH_LEFT, "- RaphProduction");
        write_paragraph(font, -1, -1, fmin(TEXT_WIDTH, fb.width), PARAGRAPH_LEFT, "- YiraSan");

        draw_frame();

        if(wait_for_the_next_slide()){
            break;
        }

        draw_image(&fb, wallpaper2_resized, TEXT_BOX_WIDTH, 0, fb.width - TEXT_BOX_WIDTH, fb.height);
        draw_rectangle(&fb, 0, 0, fmin(TEXT_BOX_WIDTH, fb.width), fb.height, TEXT_BACKGROUND);

        load_pen(font, &fb, TEXT_START, 0, TITLE_SIZE, 0, TEXT_COLOR);
        write_paragraph(font, -1, -1, fmin(TEXT_WIDTH, fb.width), PARAGRAPH_CENTER, "Github :\n");

        set_pen_size(font, TEXT_SIZE);
        write_paragraph(font, -1, -1, fmin(TEXT_WIDTH, fb.width), PARAGRAPH_JUSTIFY, "We have a GitHub repository available at the following link:\n");
        set_pen_color(font, LINK_COLOR);
        write_paragraph(font, -1, -1, fmin(TEXT_WIDTH, fb.width), PARAGRAPH_CENTER, "https://github.com/kot-org/Kot\n");
        set_pen_color(font, TEXT_COLOR);
        write_paragraph(font, -1, -1, fmin(TEXT_WIDTH, fb.width), PARAGRAPH_JUSTIFY, "Don't forget to star the GitHub repository to show your support!\n");
        
        draw_frame();

        if(wait_for_the_next_slide()){
            break;
        }

        draw_image(&fb, wallpaper3_resized, TEXT_BOX_WIDTH, 0, fb.width - TEXT_BOX_WIDTH, fb.height);
        draw_rectangle(&fb, 0, 0, fmin(TEXT_BOX_WIDTH, fb.width), fb.height, TEXT_BACKGROUND);

        load_pen(font, &fb, TEXT_START, 0, TITLE_SIZE, 0, TEXT_COLOR);
        write_paragraph(font, -1, -1, fmin(TEXT_WIDTH, fb.width), PARAGRAPH_CENTER, "Play Doom on Kot\n");

        set_pen_size(font, TEXT_SIZE);
        write_paragraph(font, -1, -1, fmin(TEXT_WIDTH, fb.width), PARAGRAPH_JUSTIFY, "Play Doom on Kot and experience the classic game in a new way!\n");
        
        draw_frame();

        if(wait_for_the_next_slide()){
            break;
        }
    }

    free_raw_image(wallpaper0_resized);
    free_raw_image(wallpaper1_resized);
    free_raw_image(wallpaper2_resized);
    free_raw_image(wallpaper3_resized);

    return EXIT_SUCCESS;
}
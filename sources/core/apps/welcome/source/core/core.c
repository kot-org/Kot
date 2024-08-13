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


kfont_t font;

int fb_fd = -1;
kframebuffer_t fb;
struct fb_fix_screeninfo fix_screeninfo;
struct fb_var_screeninfo var_screeninfo;


void draw_frame(){
    write(fb_fd, fb.buffer, fb.size);
}


int main(int argc, char* argv[]){
    fb_fd = open("/dev/fb0", O_RDWR); 

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
        draw_image(&fb, wallpaper0_resized, 600, 0, fb.width - 600, fb.height);
        draw_rectangle(&fb, 0, 0, fmin(600, fb.width), fb.height, 0x222222);

        load_pen(font, &fb, 50, 0, 64, 0, 0xffffff);
        write_paragraph(font, -1, -1, fmin(500, fb.width), PARAGRAPH_CENTER, "Welcome to Kot\n");

        set_pen_size(font, 32);
        write_paragraph(font, -1, -1, fmin(500, fb.width), PARAGRAPH_JUSTIFY, "\"Kot is nothing more than an Operating System running on x86-64. And you already know that, but we're making to turn Kot more than just something.\"\n");
        
        set_pen_size(font, 24);
        write_paragraph(font, -1, -1, fmin(500, fb.width), PARAGRAPH_RIGHT, "Kot team\n");
        draw_frame();

        sleep(5);

        draw_image(&fb, wallpaper1_resized, 600, 0, fb.width - 600, fb.height);
        draw_rectangle(&fb, 0, 0, fmin(600, fb.width), fb.height, 0x222222);

        load_pen(font, &fb, 50, 0, 64, 0, 0xffffff);
        write_paragraph(font, -1, -1, fmin(500, fb.width), PARAGRAPH_CENTER, "Credits :\n");

        set_pen_size(font, 32);
        write_paragraph(font, -1, -1, fmin(500, fb.width), PARAGRAPH_JUSTIFY, "Without the contributions of the following individuals (GitHub pseudonyms, listed alphabetically), Kot would not have existed:");
        write_paragraph(font, -1, -1, fmin(500, fb.width), PARAGRAPH_LEFT, "- 0xS3B\n");
        write_paragraph(font, -1, -1, fmin(500, fb.width), PARAGRAPH_LEFT, "- konect-V\n");
        write_paragraph(font, -1, -1, fmin(500, fb.width), PARAGRAPH_LEFT, "- Moldytzu\n");
        write_paragraph(font, -1, -1, fmin(500, fb.width), PARAGRAPH_LEFT, "- RaphProduction\n");
        write_paragraph(font, -1, -1, fmin(500, fb.width), PARAGRAPH_LEFT, "- YiraSan\n");

        draw_frame();

        sleep(5);

        draw_image(&fb, wallpaper2_resized, 600, 0, fb.width - 600, fb.height);
        draw_rectangle(&fb, 0, 0, fmin(600, fb.width), fb.height, 0x222222);

        load_pen(font, &fb, 50, 0, 64, 0, 0xffffff);
        write_paragraph(font, -1, -1, fmin(500, fb.width), PARAGRAPH_CENTER, "Github :\n");

        set_pen_size(font, 32);
        write_paragraph(font, -1, -1, fmin(500, fb.width), PARAGRAPH_JUSTIFY, "We have a GitHub repository available at the following link:\n");
        set_pen_size(font, 27);
        set_pen_color(font, 0x33ddff);
        write_paragraph(font, -1, -1, fmin(500, fb.width), PARAGRAPH_CENTER, "https://github.com/kot-org/Kot\n");
        set_pen_color(font, 0xffffff);
        write_paragraph(font, -1, -1, fmin(500, fb.width), PARAGRAPH_JUSTIFY, "Don't forget to star the GitHub repository to show your support!\n");
        
        draw_frame();

        sleep(5);

        draw_image(&fb, wallpaper3_resized, 600, 0, fb.width - 600, fb.height);
        draw_rectangle(&fb, 0, 0, fmin(600, fb.width), fb.height, 0x222222);

        load_pen(font, &fb, 50, 0, 64, 0, 0xffffff);
        write_paragraph(font, -1, -1, fmin(500, fb.width), PARAGRAPH_CENTER, "Play Doom on Kot\n");

        set_pen_size(font, 32);
        write_paragraph(font, -1, -1, fmin(500, fb.width), PARAGRAPH_JUSTIFY, "Play Doom on Kot and experience the classic game in a new way!\n");
        
        draw_frame();

        sleep(5);
    }

    free_raw_image(wallpaper0_resized);
    free_raw_image(wallpaper1_resized);
    free_raw_image(wallpaper2_resized);
    free_raw_image(wallpaper3_resized);

    return EXIT_SUCCESS;
}
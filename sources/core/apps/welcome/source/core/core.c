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

    load_pen(font, &fb, 0, 0, 64, 0, 0xffffff);
    draw_font(font, "Welcome to Kot");

    load_pen(font, &fb, 0, 0, 24, 0, 0xffffff);
    write_paragraph(font, 0, get_line_height(font) * 2 + 20, 500, PARAGRAPH_JUSTIFY, "Kot is nothing more than an Operating System running on x86-64. And you already know that, but we're making to turn Kot more than just something.");

    draw_frame();

    while(true){
        sleep(1);
    }   

    return EXIT_SUCCESS;
}
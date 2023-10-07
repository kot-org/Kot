#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

int main(int argc, char* argv[]){
    char str[100];

    printf("Enter a value: %d\n", SIZE_MAX);
    gets(str);

    // printf("\nYou entered: ");
    // puts(str);

    return 0;

    assert(argc > 0);

    int fb_fd = open("/dev/fb0", O_RDWR); 

    assert(fb_fd >= 0);

    struct fb_fix_screeninfo fix_screeninfo;
    struct fb_var_screeninfo var_screeninfo;

    assert(ioctl(fb_fd, FBIOGET_FSCREENINFO, &fix_screeninfo) == 0);
    assert(ioctl(fb_fd, FBIOGET_VSCREENINFO, &var_screeninfo) == 0);

    if( 
        fix_screeninfo.visual == FB_VISUAL_TRUECOLOR && 
        var_screeninfo.bits_per_pixel == 32 &&
        var_screeninfo.red.length == 8 && var_screeninfo.red.msb_right == 0 &&
        var_screeninfo.green.length == 8 && var_screeninfo.green.msb_right == 0 &&
        var_screeninfo.blue.length == 8 && var_screeninfo.blue.msb_right == 0 
    ){
        uint32_t* fb_buffer = malloc(fix_screeninfo.smem_len);

        for(unsigned int x = 0; x < var_screeninfo.xres; x++){
            for(unsigned int y = 0; y < var_screeninfo.yres; y++) {
                unsigned int raw_position = x + y * var_screeninfo.xres; 

                fb_buffer[raw_position] = ((y * 2) ^ (x * 2)) << var_screeninfo.red.offset;
                fb_buffer[raw_position] |= ((y * 4) ^ (x * 4)) << var_screeninfo.green.offset;
                fb_buffer[raw_position] |= (x ^ y) << var_screeninfo.blue.offset;
            }
        }

        write(fb_fd, fb_buffer, fix_screeninfo.smem_len);

        close(fb_fd);

        printf("'/dev/fb0' : %dx%d, %dbpp\n", var_screeninfo.xres, var_screeninfo.yres, var_screeninfo.bits_per_pixel);
    }else{
        printf("'/dev/fb0' : format not supported\n");
    }

    return 0;
}
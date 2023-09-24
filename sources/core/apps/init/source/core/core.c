#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <linux/fb.h>

int main(int argc, char* argv[]){
    assert(argc > 0);

    int fb_fd = open("/dev/fb0", O_RDWR); 

    assert(fb_fd >= 0);

    struct fb_fix_screeninfo fix_screeninfo;
    struct fb_var_screeninfo var_screeninfo;

    assert(ioctl(fb_fd, FBIOGET_FSCREENINFO, &fix_screeninfo) == 0);
    assert(ioctl(fb_fd, FBIOGET_VSCREENINFO, &var_screeninfo) == 0);


    void* fb_buffer = malloc(fix_screeninfo.smem_len);

    memset(fb_buffer, 0xee, fix_screeninfo.smem_len);

    write(fb_fd, fb_buffer, fix_screeninfo.smem_len);

    close(fb_fd);

    printf("%dx%d, %dbpp\n", var_screeninfo.xres, var_screeninfo.yres, var_screeninfo.bits_per_pixel);

    return 0;
}
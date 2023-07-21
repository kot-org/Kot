#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/ioctl.h>

int main(int argc, char **argv){
    struct winsize sz;

    printf("Info tty : \n");
    printf("    - Resolution : \n");
    ioctl(0, TIOCGWINSZ, &sz);
    printf("        - Symbols : %ix%i (CxR)\n", sz.ws_col, sz.ws_row);
    printf("        - Pixels: %ix%i (WxH)\n", sz.ws_xpixel, sz.ws_ypixel);

    return 0;
}
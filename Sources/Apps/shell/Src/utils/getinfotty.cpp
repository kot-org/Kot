#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/ioctl.h>

int main(int argc, char **argv){
    struct winsize* sz = (struct winsize*)malloc(sizeof(struct winsize));

    sz->ws_col = 0;
    sz->ws_row = 0;

    ioctl(0, TIOCGWINSZ, sz);
    printf("Column: %i  Row: %i\n", sz->ws_col, sz->ws_row);

    return 0;
}
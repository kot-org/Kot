#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

int main(int argc, char* argv[]){
    for(int i = 0; i < argc; i++){
        printf("%d : %s\n", i, argv[i]);
    }

    return EXIT_SUCCESS;
}
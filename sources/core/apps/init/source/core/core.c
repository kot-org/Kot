#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

int main(int argc, char* argv[]){
    while(1){
        char path[2048];
        printf("Launcher Enter the app: ");
        scanf("%s", &path);

        char* args[] = {path, NULL};

        if(execv(path, args)){
            printf("\nError loading : '%s'\n", path);
        }
    }

    return 0;
}
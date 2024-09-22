#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main(){
    const char *device = "/dev/power";
    int fd;
    int ret;

    fd = open(device, O_RDWR);
    if(fd == -1){
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    ret = ioctl(fd, 1);
    if(ret == -1){
        perror("Error during ioctl call");
        close(fd);
        return EXIT_FAILURE;
    }

    close(fd);

    printf("Your computer is not yet compatible with the Kot shutdown method.\n");
    return EXIT_SUCCESS;
}

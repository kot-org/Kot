#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

extern "C" int main(int argc, char *argv[], char *env[]){
    printf("ok");
    malloc(10);
    printf("ok");
    for(int i = 0; i < argc; i++){
        printf("\n - %s", argv[i]);
    }
    printf("ok");
    malloc(10);
    printf("ok");
    while(true){
        sleep(1);
        printf("ok");
    }
    return 0;
}
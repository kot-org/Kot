#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

extern "C" int main(int argc, char *argv[], char *env[]){
    for(int i = 0; i < argc; i++){
        printf("- %s\n", argv[i]);
    }
    while(true){
        sleep(1);
        printf("ok\n");
    }
    return 0;
}
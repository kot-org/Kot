#include <stdio.h>

extern "C" int main(int argc, char *argv[], char *env[]){
    for(int i = 0; i < argc; i++){
        printf("\n - %s", argv[i]);
    }
    return 0;
}
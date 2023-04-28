#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>

extern "C" int main(int argc, char *argv[], char *env[]){
    for(int i = 0; i < argc; i++){
        printf("- %s\n", argv[i]);
    }
    while(true){
        char c = getchar();
        printf("%x\n", rand());
    }
    return 0;
}
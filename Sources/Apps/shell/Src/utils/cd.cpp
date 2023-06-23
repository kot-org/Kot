#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "system.h"


int main(int argc, char* argv[]) {
    char* DirPath = argv[1];

    if(argc != 2){
        printf("Usage : cd <folder-path>\n");
        return EXIT_FAILURE;
    }

    if(chdir(DirPath)){
        printf("Directory : '%s' not found\n", DirPath);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
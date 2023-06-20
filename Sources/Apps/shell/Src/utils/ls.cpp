#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    DIR* Directory;
    struct dirent* Entry;

    char* DirPath = "d1:.";

    if(argc >= 2){
        DirPath = argv[1];
    }

    Directory = opendir(DirPath);

    if(!Directory){
        printf("Directory : '%s' not found\n", DirPath);
        return EXIT_FAILURE;
    }

    while((Entry = readdir(Directory)) != NULL) {
        printf("%s\n", Entry->d_name);
    }

    closedir(Directory);

    return EXIT_SUCCESS;
}
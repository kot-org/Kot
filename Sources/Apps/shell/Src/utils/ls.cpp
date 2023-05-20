#include <stdio.h>
#include <dirent.h>

int main(int argc, char* argv[]) {
    DIR* Directory;
    struct dirent* Entry;

    // TODO: opendir avec arg
    Directory = opendir(".");

    while((Entry = readdir(Directory)) != NULL) {
        printf("%s\n", Entry->d_name);
    }

    closedir(Directory);

    return 0;
}
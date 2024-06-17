#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "uninstall.h"

static bool is_dir_exist(char* path){
    struct stat sb;
    return (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode));
}

static int delete(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){
    return remove(fpath);
}

int rmrf(char* path){
    return nftw(path, delete, 64, FTW_DEPTH | FTW_PHYS);
}

int uninstall_app(char* name){
    printf("Uninstalling %s...\n", name);
    char* path_store = getenv("PATHSTORE");
    if(is_dir_exist(path_store)){
        char* path_store_app = malloc(strlen(path_store) + sizeof((char)'/') + strlen(name) + sizeof((char)'/') + 1);
        strcpy(path_store_app, path_store);
        strcat(path_store_app, "/");
        strcat(path_store_app, name);
        strcat(path_store_app, "/");

        if(is_dir_exist(path_store_app)){
            printf("Done uninstall %s\n", name);
            rmrf(path_store_app);
            rmdir(path_store_app);
            return 0;
        }
    }

    printf("Can't uninstall %s\n", name);

    return -1;
}
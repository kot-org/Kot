#include <stdio.h>
#include <assert.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "remove.h"
#include "../icon/icon.h"

#define MAX_REMOVE_DIR 256

static bool is_dir_exist(char* path){
    struct stat sb;
    return (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode));
}

static bool check_user_allow(char* name){
    char allow_remove[3];
    printf("%s > Would you like to remove it? (Y/N)\n", name);
    fgets(allow_remove, sizeof(allow_remove), stdin);
    allow_remove[strcspn(allow_remove, "\n")] = 0;
    return (!strcmp("Y", allow_remove));
}

static int remove_directory(char* path){
    DIR* d = opendir(path);
    if(!d){
        return -1;
    } 

    struct dirent* p;
    int count = 0;
    char* path_list[MAX_REMOVE_DIR];

    while((p = readdir(d)) != NULL){
        int r;
        char* buffer;
        size_t path_len = strlen(path);


        if(asprintf(&path_list[count], "%s%s", path, p->d_name) >= 0){
            count++;
            if(count >= MAX_REMOVE_DIR){
                remove_directory(path);
                break;
            }
        }
    }

    closedir(d);

    for(int i = 0; i < count; i++){
        printf("Removing : %s\n", path_list[i]);
        struct stat statbuf;
        if(lstat(path_list[i], &statbuf) >= 0){
            if(S_ISDIR(statbuf.st_mode)) {
                remove_directory(path_list[i]);
            }else{
                remove(path_list[i]);
            }
        }
        free(path_list[i]);
    }

    printf("Removing : %s\n", path);
    return rmdir(path);
}

int remove_app(char* name, bool check_user){
    char* path_store = getenv("PATHSTORE");
    if(is_dir_exist(path_store)){
        char* path_store_app;
        assert(asprintf(&path_store_app, "%s/%s/", path_store, name) >= 0);

        if(is_dir_exist(path_store_app)){
            bool do_remove = true;

            if(check_user){
                do_remove = check_user_allow(name);
            }

            if(do_remove){
                printf("Removing %s\n", name);
                remove_directory(path_store_app);
                remove_element_from_json(name);
                printf("Done remove %s\n", name);
                return 0;
            }else{
                printf("Cancel the removal of %s\n", name);
                return -2;
            }
        }
    }

    printf("Can't find : ' %s\n", name);
    return -1;
}
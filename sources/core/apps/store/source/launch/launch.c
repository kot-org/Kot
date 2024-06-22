#include <stdio.h>
#include <assert.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <cjson/cJSON.h> 

#include "launch.h"

static bool is_dir_exist(char* path){
    struct stat sb;
    return (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode));
}

static bool is_file_exists(char* path){
    struct stat sb;   
    return (stat(path, &sb) == 0);
}

static int get_executable_path(char* app_info_path, char** relative_path){
    FILE* fp = fopen(app_info_path, "r");

    if(fp != NULL){
        fseek(fp, 0, SEEK_END);
        size_t size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        
        void* buffer = malloc(size);
        int len = fread(buffer, 1, size, fp); 
        fclose(fp); 
    
        cJSON* root = cJSON_Parse(buffer); 
        if(root != NULL){
            cJSON* executable_path = cJSON_GetObjectItem(root, "executable_path");
            if(executable_path != NULL){
                if(cJSON_IsString(executable_path) && (executable_path->valuestring != NULL)){
                    *relative_path = malloc(strlen(executable_path->valuestring) + 1);
                    strcpy(*relative_path, executable_path->valuestring);

                    cJSON_Delete(root); 
                    free(buffer);
                    return 0;
                }
            }
            cJSON_Delete(root); 
        }

        free(buffer);
        return -1;
    }else{
        return -1;
    }
}

int launch_app(char* name, int argc, char *argv[]){
    char* path_store = getenv("PATHSTORE");
    if(is_dir_exist(path_store)){
        char* path_store_app;
        assert(asprintf(&path_store_app, "%s/%s/", path_store, name) >= 0);

        if(is_dir_exist(path_store_app)){
            char* path_store_app_info_json;
            assert(asprintf(&path_store_app_info_json, "%sapp-info.json", path_store_app) >= 0);

            char* relative_path = NULL;
            int result = get_executable_path(path_store_app_info_json, &relative_path);
            free(path_store_app_info_json);

            if(!result){
                char* absolute_path = NULL;
                assert(asprintf(&absolute_path, "%s%s", path_store_app, relative_path) >= 0);

                printf("Launching : %s\n", absolute_path);
                int r = execve(absolute_path, argv, NULL);
                printf("Error when launching : %s, error code : %d\n", absolute_path, r);

                free(relative_path);
                free(absolute_path);
            }

            printf("Error when launching : %d\n", name);
            free(path_store_app);

            return -1;
        }

        free(path_store_app);
    }

    printf("Can't find the app named : %d\n", name);

    return -1;
}
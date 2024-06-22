#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "install.h"
#include "../deps/deps.h"
#include "../untar/untar.h"
#include "../download/download.h"

static bool is_dir_exist(char* path){
    struct stat sb;
    return (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode));
}

static int create_dir_if_not_exist(char* path){
    if(!is_dir_exist(path)){
        mkdir(path, 0777);
    }

    return 0;
}

int install_app(CURL* curl, char* url, char* name, bool reinstall){
    char* path_store = getenv("PATHSTORE");
    create_dir_if_not_exist(path_store);

    char* path_store_app;
    assert(asprintf(&path_store_app, "%s/%s/", path_store, name) >= 0);

    create_dir_if_not_exist(path_store_app);

    char* path_store_app_info_json;
    assert(asprintf(&path_store_app_info_json, "%sapp-info.json", path_store_app) >= 0);

    if(download_file(curl, url, path_store_app_info_json, reinstall)){
        printf("Error: Aborting installation of %s!\n", name);

        free(path_store_app_info_json);
        free(path_store_app);
        return -1;
    }
    
    printf("Checking dependencies...\n");
    char* installation_file_url = NULL;
    int check_deps = check_dependencies(path_store_app_info_json, &installation_file_url);
    if(check_deps == -2){
        printf("Aborting installation of %s!\n", name);
        rmdir(path_store_app);

        free(path_store_app_info_json);
        free(path_store_app);
        return -1;
    }if(check_deps == -1){
        char force_install[3];
        printf("Some dependencies required for installation are not found. If you choose to continue, the installation may not work as expected. Do you still want to proceed with the installation? (Y/N)\n");
        fgets(force_install, sizeof(force_install), stdin);
        force_install[strcspn(force_install, "\n")] = 0;
        if(strcmp("Y", force_install)){
            printf("Aborting installation of %s!\n", name);
            
            remove(path_store_app_info_json);
            rmdir(path_store_app);

            free(path_store_app_info_json);
            free(path_store_app);
            return -1;
        }
    }else{
        printf("All dependencies found\n");
    }

    if(installation_file_url != NULL){
        char* installation_file_name = strrchr(installation_file_url, '/');
        if(installation_file_name != NULL){
            installation_file_name++;
            char* path_store_installation_file = malloc(strlen(path_store_app) + strlen(installation_file_name) + 1);
            strcpy(path_store_installation_file, path_store_app);
            strcat(path_store_installation_file, installation_file_name);

            if(download_file(curl, installation_file_url, path_store_installation_file, reinstall)){
                printf("Error: Aborting installation of %s!\n", name);

                remove(path_store_app_info_json);
                rmdir(path_store_app);

                free(installation_file_url);
                free(path_store_installation_file);
                free(path_store_app_info_json);
                free(path_store_app);
                return -1;
            }

            untar(path_store_installation_file, path_store_app);
        }
    }

    free(path_store_app_info_json);
    free(path_store_app);

    return 0;
}
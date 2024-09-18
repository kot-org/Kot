#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "install.h"
#include "../deps/deps.h"
#include "../icon/icon.h"
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

    if(is_dir_exist(path_store_app) && !reinstall){
        printf("\033[0;31mError: The application : '%s' is already installed.\033[0m\n", name);
        return -2;
    }

    create_dir_if_not_exist(path_store_app);

    char* path_store_app_info_json;
    assert(asprintf(&path_store_app_info_json, "%sapp-info.json", path_store_app) >= 0);

    if(download_file(curl, url, path_store_app_info_json, reinstall)){
        printf("\033[0;31mError: Aborting installation of %s!\033[0m\n", name);

        free(path_store_app_info_json);
        free(path_store_app);
        return -1;
    }
    
    printf("Checking dependencies...\n");
    char* installation_file_url = NULL;
    char* executable_relative_path = NULL;
    int check_deps = check_dependencies(path_store_app_info_json, &installation_file_url, &executable_relative_path);
    if(check_deps == -2){
        printf("\033[0;31mError : Aborting installation of %s!\033[0m\n", name);
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
            printf("\033[0;31Error : Aborting installation of %s!\033[0m\n", name);
            
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
                printf("\033[0;31Error: Aborting installation of %s!\033[0m\n", name);

                remove(path_store_app_info_json);
                rmdir(path_store_app);

                free(installation_file_url);
                free(path_store_installation_file);
                free(path_store_app_info_json);
                free(path_store_app);
                return -1;
            }

            char* path_executable_file = malloc(strlen(path_store_app) + strlen(executable_relative_path) + 1);
            strcpy(path_executable_file, path_store_app);
            strcat(path_executable_file, executable_relative_path);
            add_element_to_json(path_store_app, "", path_executable_file, name);

            untar(path_store_installation_file, path_store_app);
        }
    }

    free(path_store_app_info_json);
    free(path_store_app);

    printf("\033[0;32mApp successfully installed\033[0m\n");

    return 0;
}

char** get_installed_apps(char* name){
    char* path_store = getenv("PATHSTORE");
    if(path_store == NULL){
        return NULL;
    }

    if(!is_dir_exist(path_store)){
        return NULL;
    }

    DIR* dir = opendir(path_store);

    if(dir == NULL){
        return NULL;
    }

    struct dirent *entry;
    int count = 0;
    char **apps = NULL;

    while((entry = readdir(dir)) != NULL){
        if(entry->d_type == DT_DIR && strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")){
            count++;
        }
    }

    apps = (char **)malloc((count + 1) * sizeof(char *));
    if(apps == NULL){
        closedir(dir);
        return NULL;
    }

    rewinddir(dir);

    int index = 0;
    while((entry = readdir(dir)) != NULL){
        if(entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
            if(name == NULL || strcmp(entry->d_name, name) == 0){
                apps[index] = strdup(entry->d_name);
                index++;
            }
        }
    }

    closedir(dir);

    apps[index] = NULL;

    return apps;
}

void free_get_installed_apps(char** apps){
    if(apps != NULL){
        for(int i = 0; apps[i] != NULL; i++){
            free(apps[i]);
        }
        free(apps);
    }
}
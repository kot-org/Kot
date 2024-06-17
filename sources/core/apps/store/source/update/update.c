#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <curl/curl.h>
#include <cjson/cJSON.h> 

#include "update.h"
#include "../install/install.h"

static bool is_dir_exist(char* path){
    struct stat sb;
    return (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode));
}

static bool is_file_exists(char* path){
    struct stat sb;   
    return (stat(path, &sb) == 0);
}

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp){
    fetch_app_info_t* buffer_info = (fetch_app_info_t*)userp;
    size_t real_size = size * nmemb;

    buffer_info->size += real_size;
    buffer_info->buffer = realloc(buffer_info->buffer, buffer_info->size);
    
    memcpy(&(buffer_info->buffer[buffer_info->size - real_size]), contents, real_size);

    return real_size;
}

static int get_version_from_string(char* version_str){
    int x, y, z;
    int len = strlen(version_str);

    for(int i = 0; i < len; i++){
        if(version_str[i] == '.'){
            continue;
        }
        if(version_str[i] < '0' || version_str[i] > '9'){
            return -1;
        }
    }

    sscanf(version_str, "%d.%d.%d", &x, &y, &z);

    const int x_shift = 16;
    const int y_shift = 8;
    const int x_mask = 0xFFFF0000;
    const int y_mask = 0x0000FF00;
    const int z_mask = 0x000000FF;

    int version_int = (x << x_shift) & x_mask;
    version_int |= (y << y_shift) & y_mask;
    version_int |= z & z_mask;

    return version_int;
}

static int get_version_url(CURL* curl, char* url_info_path){
    fetch_app_info_t* buffer_info = malloc(sizeof(fetch_app_info_t));
    buffer_info->buffer = NULL;
    buffer_info->size = 0;
    curl_easy_setopt(curl, CURLOPT_URL, url_info_path);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer_info);
    curl_easy_setopt(curl, CURLOPT_CAINFO, "/usr/etc/ssl/cert.pem");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    CURLcode res = curl_easy_perform(curl);
    
    if(res != CURLE_OK){
        fprintf(stderr, "Error : curl_%s\n",  curl_easy_strerror(res));
        free(buffer_info->buffer);
        free(buffer_info);
        return -1;
    }else{
        cJSON* root = cJSON_Parse(buffer_info->buffer);
        if(root != NULL){
            cJSON* version = cJSON_GetObjectItem(root, "version");
            if(version != NULL){
                if(cJSON_IsString(version) && (version->valuestring != NULL)){ 
                    int ret = get_version_from_string(version->valuestring);
                    cJSON_Delete(root);

                    free(buffer_info->buffer);
                    free(buffer_info);
                    return ret;
                }
            }
        }
        cJSON_Delete(root);

        free(buffer_info->buffer);
        free(buffer_info);
    }

    return -1;
}

static int check_update(CURL* curl, char* app_info_path, char** url){
    FILE* fp = fopen(app_info_path, "r");

    int version_local = 0;
    int version_url = 0;
    int n_version_get = 0;

    if(fp != NULL){
        fseek(fp, 0, SEEK_END);
        size_t size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        
        void* buffer = malloc(size);
        int len = fread(buffer, 1, size, fp); 
        fclose(fp); 
    
        cJSON* root = cJSON_Parse(buffer); 
        if(root != NULL){
            cJSON* app_info_url = cJSON_GetObjectItem(root, "app_info_url");
            if(app_info_url != NULL){
                if(cJSON_IsString(app_info_url) && (app_info_url->valuestring != NULL)){     
                    version_url = get_version_url(curl, app_info_url->valuestring);
                    if(version_url > 0){
                        n_version_get++;
                    }

                    *url = malloc(strlen(app_info_url->valuestring) + 1);
                    strcpy(*url, app_info_url->valuestring);
                }
            }
            cJSON* version = cJSON_GetObjectItem(root, "version");
            if(version != NULL){
                if(cJSON_IsString(version) && (version->valuestring != NULL)){ 
                    version_local = get_version_from_string(version->valuestring);
                    if(version_local > 0){
                        n_version_get++;
                    }
                }
            }
            cJSON_Delete(root); 
        }

        free(buffer);

        if(n_version_get == 2){
            if(version_url > version_local){
                return 1;
            }else{
                return 0;
            }
        }else{
            return -1;
        }
    }else{
        return -1;
    }
}

int update_app(CURL* curl, char* name){
    char* path_store = getenv("PATHSTORE");
    if(is_dir_exist(path_store)){
        char* path_store_app = malloc(strlen(path_store) + sizeof((char)'/') + strlen(name) + sizeof((char)'/') + 1);
        strcpy(path_store_app, path_store);
        strcat(path_store_app, "/");
        strcat(path_store_app, name);
        strcat(path_store_app, "/");

        if(is_dir_exist(path_store_app)){
            char* path_store_app_info_json = malloc(strlen(path_store_app) + strlen("app-info.json") + 1);
            strcpy(path_store_app_info_json, path_store_app);
            strcat(path_store_app_info_json, "app-info.json");

            char* url = NULL;
            int result = check_update(curl, path_store_app_info_json, &url);

            if(result == 0){
                printf("%s : is up-to-date\n", name);
            }else if(result == 1 && url != NULL){
                printf("%s : is not up-to-date\n", name);
                printf("Updating...\n");
                int r = install_app(curl, url, name, true);
                if(!r){
                    printf("Update finish with success\n");
                }else{
                    printf("Update finish with error code : %d\n", r);
                }
            }else{
                printf("%s : is not installed corectly\n", name);
            }
        }else{
            printf("%s : is not installed\n", name);
        }
    }else{
        printf("%s : is not installed\n", name);
    }
}
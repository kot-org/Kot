#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h> 

#include "apps.h"

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp){
    fetch_apps_data_t* buffer_info = (fetch_apps_data_t*)userp;
    size_t real_size = size * nmemb;

    buffer_info->size += real_size;
    buffer_info->buffer = realloc(buffer_info->buffer, buffer_info->size);
    
    memcpy(&(buffer_info->buffer[buffer_info->size - real_size]), contents, real_size);

    return real_size;
}

fetch_apps_data_t* fetch_apps_data(void){
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl){
        fetch_apps_data_t* buffer_info = malloc(sizeof(fetch_apps_data_t));
        buffer_info->buffer = NULL;
        buffer_info->size = 0;
        curl_easy_setopt(curl, CURLOPT_URL, "https://kot-store.github.io/apps/");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer_info);
        curl_easy_setopt(curl, CURLOPT_CAINFO, "/usr/etc/ssl/cert.pem");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        res = curl_easy_perform(curl);

        if(res != CURLE_OK){
            fprintf(stderr, "Error : curl_%s\n",  curl_easy_strerror(res));
            free(buffer_info->buffer);
            free(buffer_info);
            return NULL;
        }else{
            return buffer_info;
        }

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    return NULL;
}

char* find_apps_url_by_name(char* name){
    fetch_apps_data_t* data_info = fetch_apps_data();
    if(data_info != NULL){
        cJSON* root = cJSON_Parse(data_info->buffer);
        cJSON* applications = cJSON_GetObjectItem(root, "applications");
        
        for(int i = 0 ; i < cJSON_GetArraySize(applications); i++){
            cJSON* application = cJSON_GetArrayItem(applications, i);
            cJSON* application_name = cJSON_GetObjectItem(application, "name");
            if(cJSON_IsString(application_name) && (application_name->valuestring != NULL)){ 
                if(!strcmp(name, application_name->valuestring)){
                    cJSON* json_link_application_url = cJSON_GetObjectItem(application, "json_link");
                    if(cJSON_IsString(json_link_application_url) && (json_link_application_url->valuestring != NULL)){ 
                            char* return_value = malloc(strlen(json_link_application_url->valuestring) + 1);
                            strcpy(return_value, json_link_application_url->valuestring);
                            cJSON_Delete(root); 

                            free(data_info->buffer);
                            free(data_info);

                            return return_value;
                    }
                }
            } 
        }

        cJSON_Delete(root); 

        free(data_info->buffer);
        free(data_info);
    }

    return NULL;
}

char** find_apps_url_by_tag(char* tag){
    fetch_apps_data_t* data_info = fetch_apps_data();
    char** return_value = NULL;

    if(data_info != NULL){
        cJSON* root = cJSON_Parse(data_info->buffer);
        cJSON* applications = cJSON_GetObjectItem(root, "applications");
        
        size_t return_value_count_url = 0;
        for(int i = 0 ; i < cJSON_GetArraySize(applications); i++){
            cJSON* application = cJSON_GetArrayItem(applications, i);
            cJSON* application_tags = cJSON_GetObjectItem(application, "tags");

            for(int y = 0 ; y < cJSON_GetArraySize(applications); y++){
                cJSON* application_tag = cJSON_GetArrayItem(application_tags, y);
                if(cJSON_IsString(application_tag) && (application_tag->valuestring != NULL)){ 
                    if(!strcmp(tag, application_tag->valuestring)){
                        cJSON* json_link_application_url = cJSON_GetObjectItem(application, "json_link");
                        if(cJSON_IsString(json_link_application_url) && (json_link_application_url->valuestring != NULL)){ 
                            return_value = realloc(return_value, (return_value_count_url + 2) * sizeof(char*));
                            return_value[return_value_count_url + 1] = NULL;
                            return_value[return_value_count_url] = malloc(strlen(json_link_application_url->valuestring) + 1);
                            strcpy(return_value[return_value_count_url], json_link_application_url->valuestring);
                            return_value_count_url++;
                        }
                    }
                }
            }
        }

        cJSON_Delete(root); 

        free(data_info->buffer);
        free(data_info);
    }

    return return_value;
}
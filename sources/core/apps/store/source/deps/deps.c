#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <cjson/cJSON.h> 

#include "deps.h"

static bool is_file_exists(char* path){
  struct stat sb;   
  return (stat(path, &sb) == 0);
}

int check_dependencies(char* app_info_json_path, char** installation_file_url){
    FILE* fp = fopen(app_info_json_path, "r"); 

    if(fp == NULL){ 
        printf("Error: Unable to open the file : %s\n", app_info_json_path); 
        return -1; 
    } 
  
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    void* buffer = malloc(size);
    int len = fread(buffer, 1, size, fp); 
    fclose(fp); 
  
    cJSON* root = cJSON_Parse(buffer); 

    int r = 0;

    if(root != NULL){ 
        cJSON* dependencies = cJSON_GetObjectItem(root, "dependencies_file");
        if(dependencies != NULL){
            for(int i = 0 ; i < cJSON_GetArraySize(dependencies); i++){
                cJSON* dependency = cJSON_GetArrayItem(dependencies, i);
                if(cJSON_IsString(dependency) && (dependency->valuestring != NULL)){ 
                    if(!is_file_exists(dependency->valuestring)){
                        printf("Depency %s not found !\n", dependency->valuestring);
                        r = -1;
                        break;
                    }
                }
            }
        }

        cJSON* installation_file = cJSON_GetObjectItem(root, "installation_file");
        if(cJSON_IsString(installation_file) && (installation_file->valuestring != NULL)){
            *installation_file_url = malloc(strlen(installation_file->valuestring) + 1);
            strcpy(*installation_file_url, installation_file->valuestring);
        }
    }else{
        const char *error_ptr = cJSON_GetErrorPtr(); 
        if(error_ptr != NULL){ 
            printf("Error: %s\n", error_ptr); 
        }  
        r = -1;      
    }
  

    cJSON_Delete(root); 
    free(buffer);
    return r;     
}
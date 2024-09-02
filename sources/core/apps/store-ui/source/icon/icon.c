#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

int add_element_to_json(const char* cwd_path, const char* icon_path, const char* executable_path, const char* app_name){
    printf("Adding app icon to the desktop...\n");

    const char* file_path = "/usr/bin/icons/icons.json";
    FILE *file = fopen(file_path, "r+");
    if (!file) {
        perror("Failed to open file");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *file_content = (char *)malloc(file_size + 1);
    if (!file_content) {
        perror("Failed to allocate memory");
        fclose(file);
        return -1;
    }

    fread(file_content, 1, file_size, file);
    file_content[file_size] = '\0';

    cJSON* root = cJSON_Parse(file_content);
    if (!root) {
        fprintf(stderr, "Failed to parse JSON\n");
        free(file_content);
        fclose(file);
        return -1;
    }

    cJSON* new_element = cJSON_CreateObject();
    cJSON_AddStringToObject(new_element, "cwdPath", cwd_path);
    cJSON_AddStringToObject(new_element, "iconPath", icon_path);
    cJSON_AddStringToObject(new_element, "executablePath", executable_path);
    cJSON_AddStringToObject(new_element, "appName", app_name);

    cJSON_AddItemToArray(root, new_element);

    char* updated_json = cJSON_Print(root);
    fseek(file, 0, SEEK_SET);
    fprintf(file, "%s", updated_json);

    free(file_content);
    free(updated_json);
    cJSON_Delete(root);
    fclose(file);

    printf("Icon added successfully\n");

    return 0;
}
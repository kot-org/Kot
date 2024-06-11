#ifndef APPS_H
#define APPS_H

#include <stddef.h>

typedef struct{
    char* buffer;
    size_t size;
}fetch_apps_data_t;

fetch_apps_data_t* fetch_apps_data(void);
char* find_apps_url_by_name(char* name);
char** find_apps_url_by_tag(char* tag);

#endif // APPS_H
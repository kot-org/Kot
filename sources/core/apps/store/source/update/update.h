#ifndef UPDATE_H
#define UPDATE_H

typedef struct{
    char* buffer;
    size_t size;
}fetch_app_info_t;

#include <stddef.h>
#include <curl/curl.h>

int update_app(CURL* curl, char* name);

#endif // UPDATE_H
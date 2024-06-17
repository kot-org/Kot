#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <stddef.h>
#include <curl/curl.h>

int download_file(CURL* curl, char* url, char* path, bool force_download);

#endif // DOWNLOAD_H
#ifndef INSTALL_H
#define INSTALL_H

#include <stddef.h>
#include <curl/curl.h>

int install_app(CURL* curl, char* url, char* name);

#endif // INSTALL_H
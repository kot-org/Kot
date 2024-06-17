#ifndef INSTALL_H
#define INSTALL_H

#include <stddef.h>
#include <curl/curl.h>

int install_app(CURL* curl, char* url, char* name, bool reinstall);

#endif // INSTALL_H
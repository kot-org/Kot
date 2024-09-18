#ifndef INSTALL_H
#define INSTALL_H

#include <stddef.h>
#include <curl/curl.h>

int install_app(CURL* curl, char* url, char* name, bool reinstall);
char** get_installed_apps(char* name);
void free_get_installed_apps(char** apps);

#endif // INSTALL_H
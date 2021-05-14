#ifndef _DIRENT_H
#define _DIRENT_H

#define NAME_MAX 255

struct dirent {
 char d_name[NAME_MAX+1];
};

typedef struct DIR DIR;

DIR *opendir(const char *name);
struct dirent *readdir(DIR *dirp);
int closedir(DIR *dirp);

#endif

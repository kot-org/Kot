#ifndef _SYS_DIRENT_H
#define _SYS_DIRENT_H


#define NAME_MAX 255

struct dirent {
 char d_name[NAME_MAX+1];
};

struct dir {
        struct dirent current_dentry;
	int fd;
	char buf[20];
};
/*
struct dir *opendir(char *name);
struct dirent *readdir(struct dir *dirp);
int closedir(struct dir *dirp);
*/
#endif

#ifndef _STDLIB_H
#define _STDLIB_H
#include <sys/dirent.h>
#include <sys/defs.h>

int main(int argc, char *argv[], char *envp[]);
void exit(int status);

void *malloc(size_t size);
void free(void *ptr);


//stdn functions
size_t read(int fd, void *buf, size_t count);
size_t write(int fd, const void *buf, size_t count);
int close(int fd);

//process functions
int kill(int pid);
uint64_t sleep(uint64_t seconds);
pid_t fork(void);
pid_t waitpid(pid_t pid, int *status, int options);
int execve(const char *file, char *const argv[], char *const envp[]);
void exit(int arg1);
uint64_t sleep(uint64_t seconds);
int process_status();

//path functions
char *getcwd(char *buf, size_t size);
int chdir(const char *path);
void changedir(char *path);


//file funtions
enum { O_RDONLY = 0, O_WRONLY = 1, O_RDWR = 2, O_CREAT = 0x40, O_DIRECTORY = 0x10000 };
uint64_t open(const char *pathname, int flags);

struct dir *opendir(char *name);
//struct dirent *readdir(struct dir *dirp);
int closedir(struct dir *dirp);
struct dirent *readdir(char *dir);
int kill(int pid_t);
#endif

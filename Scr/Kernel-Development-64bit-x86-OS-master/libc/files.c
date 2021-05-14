#include<stdlib.h>
#include<syscall.h>
#include<sys/syscall.h>
#include<sys/dirent.h>

size_t write(int arg1,const void *arg2, size_t arg3) {
        ssize_t res;
        if (arg1 < 0) {
              return 0;
        }

        res = (size_t )syscall_3(s_write,(uint64_t)arg1,(uint64_t)arg2,(uint64_t)arg3);
        return res;
}

size_t read(int fd, void *buf, size_t count) {
        ssize_t res;
        res= (size_t)syscall_3(s_read,(uint64_t)fd,(uint64_t)buf,(uint64_t)count);
        return res;
}
int close(int fd){
        int res;
        if(fd <0){
        return 0;
        }

        res = (int)syscall_1(s_close,(uint64_t)fd);     
        return res;
}

uint64_t open(const char *pathname,int flags)
{
        uint64_t res;
        res = (uint64_t)syscall_2(s_open,(uint64_t)pathname,(uint64_t)flags);
        return res;
}

struct dirent *readdir(char *d){
	struct dirent *dire = (struct dirent*)syscall_1(s_readdir,(uint64_t)d);
	return dire;
}

struct dir* opendir(char *name)
{
        
        struct dir *d;
	d = (struct dir *)syscall_1(s_opendir,(uint64_t)name);
	return d;
}
int closedir(struct dir *d)
{ 
	syscall_1(s_closedir,(uint64_t)d);
        return -1;
}


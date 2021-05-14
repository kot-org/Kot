#include <stdlib.h>
#include <syscall.h>
#include <sys/syscall.h>

int chdir(const char *path)
{
          return syscall_1(s_chdir, (uint64_t)path);
}

char* getcwd(char *buf, size_t size)
{
        char *cbuf;
        cbuf = (char *) syscall_2(s_getcwd,(uint64_t)buf,(uint64_t)size);
        return cbuf;
}

void changedir(char *path)
{
	syscall_1(s_changedir,(uint64_t)path);
}

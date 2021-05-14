#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/defs.h>
#include <syscall.h>
#include <stdio.h>
//This file has all the methods of process calls  


pid_t fork(void)
{
  pid_t return_code;
        return_code = (pid_t) syscall_0(s_fork);

        return return_code;
}

void exit(int arg1)
{
        syscall_0(s_exit);
}

int execve(const char *file, char *const argv[], char *const envp[])
{  
        const char *temp = file;
        while (temp && !((*temp == 'b') && (*(temp+1) == 'i') && (*(temp+2) == 'n'))){
        temp++;
        }
        if (temp == NULL || (*temp == '\0') || (*(temp+4) == '\0')) {
        puts("Such binary file doesnt exist");
        return -1;
        }
        int return_code = (int)syscall_3(s_execve,(uint64_t)temp,(uint64_t)argv,(uint64_t)envp);
     
        return return_code;
}

pid_t waitpid(pid_t pid, int *status, int options)
{
        return (pid_t) syscall_3(s_waitpid, (uint64_t)pid, (uint64_t)status,(uint64_t)options);
}

uint64_t sleep(uint64_t secs)
{
        uint64_t ret;
        uint64_t ticks = secs*1000;
        ret =(uint64_t)syscall_1(s_sleep,(uint64_t)ticks);
        return ret;
}

int kill(int pid_t)
{
        return (int)syscall_1(s_kill,(uint64_t)pid_t);
}

int process_status()
{
        return (int) syscall_1(s_listprocess,(uint64_t)0);
}

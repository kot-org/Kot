#ifndef _SYS_SYSCALL_H
#define _SYS_SYSCALL_H
#include <sys/interrupt.h>

// Intel Machine Specific Registers 
#define MSR_EFER    0xC0000080
#define MSR_STAR    0xC0000081
#define MSR_LSTAR   0xC0000082
#define MSR_CSTAR   0xC0000083
#define MSR_SFMASK  0xC0000084

#define Invalid_FD(fd) ((fd) < 0 || (fd) >= 64)

//file
#define s_read		0
#define s_write		1
#define s_open		2
#define s_close		3
//directory
#define s_opendir	4
#define s_readdir	5
#define s_closedir	15
#define s_chdir		10
#define s_getcwd	11

//process
#define s_fork		6
#define s_execve	7
#define s_waitpid	8
#define s_sleep		9
#define s_listprocess	12  
#define s_kill		13	
#define s_exit		14
#define s_yield         16
#define s_changedir	150

uint64_t sys_exit();
void syscall_handler(registers_t *r);
void syscall_init();
int sys_read(uint64_t fd,char *buf,uint64_t len);
//int sys_read(uint64_t fd_count,void * addr,uint64_t len);
int sys_write(uint64_t fd_count,void * addr, int len);
uint64_t sys_open(char *file_name, int flags);
int sys_close(uint64_t fd);
//int sys_fork_function();

//directory syscalls
int sys_yield();
int sys_readdir(char* direct);
int sys_opendir(char *dir_nam);
void sys_closedir(uint64_t dir);
int sys_kill(uint64_t pid);

void sys_changedir(uint64_t path);

#endif

#pragma once

#include <scheduler/scheduler.h>

#if __WORDSIZE == 64
typedef long int           pid_t;
#else
typedef long long int      pid_t;
#endif

struct wait_pid_t{
    pid_t Pid;
    struct kthread_t* Thread;
};
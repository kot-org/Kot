#pragma once

#include <abi-bits/pid_t.h>

#include <scheduler/scheduler.h>


struct wait_pid_t{
    pid_t Pid;
    struct kthread_t* Thread;
};
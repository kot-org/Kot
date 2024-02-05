#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <asm/prctl.h>
#include <kot/syscall.h>
#include <bits/ensure.h>
#include <frg/vector.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/elf/startup.h>
#include <mlibc/allocator.hpp>
#include <mlibc/all-sysdeps.hpp>

extern "C" void __mlibc_signal_restore();

namespace mlibc {
    int sys_sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve){
        auto result = do_syscall(SYS_SIGPROCMASK, how, set, retrieve);

        if(result < 0){
            return -result;
        }

        return 0;
    }

    int sys_sigaction(int signal, const struct sigaction *__restrict action, struct sigaction *__restrict old_action){
        void* sigreturn = (void*)__mlibc_signal_restore;

        auto result = do_syscall(SYS_SIGACTION, signal, action, sigreturn, old_action);

        if(result < 0){
            return -result;
        }


        return 0;
    }
}
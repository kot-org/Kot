#include <stdio.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <kot/thread.h>
#include <bits/ensure.h>
#include <mlibc/tcb.hpp>
#include <mlibc/tid.hpp>
#include <frg/array.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/thread.hpp>
#include <frg/allocation.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/posix-sysdeps.hpp>

extern "C" {

Tcb *__rtdl_allocateTcb();

// Warning: This function must be used after each kot_Sys_CreateThread unless another initialization for the thread has been performed
KResult kot_InitializeThread(kot_thread_t thread){
    auto new_tcb = __rtdl_allocateTcb();
    pid_t tid = 0;
    kot_Sys_Thread_Info_Get(thread, 0, (uint64_t*)&tid);
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    // TODO: due to alignment guarantees, the stackaddr and stacksize might change
    // when the stack is allocated. Currently this isn't propagated to the TCB,
    // but it should be.
    void *stack = attr.__mlibc_stackaddr;
    if (!mlibc::sys_prepare_stack) {
        MLIBC_MISSING_SYSDEP();
        return ENOSYS;
    }
    void *entry;
    kot_Sys_Thread_Info_Get(thread, 3, (uint64_t*)&entry);
    int ret = mlibc::sys_prepare_stack(&stack, reinterpret_cast<void*>(entry),
            NULL, new_tcb, &attr.__mlibc_stacksize, &attr.__mlibc_guardsize);
    if (ret)
        return ret;

    new_tcb->stackSize = attr.__mlibc_stacksize;
	new_tcb->guardSize = attr.__mlibc_guardsize;
    new_tcb->stackAddr = stack;  
    new_tcb->tid = tid;

    kot_Sys_SetTCB(thread, (void*)new_tcb);
    return KSUCCESS; 
}

KResult kot_SetupStack(void** Data, size64_t* Size, int argc, char** argv, char** envp){
    size64_t args = 0;
    for(int i = 0; i < argc; i++){
        args += strlen(argv[i]) + 1; // Add NULL char at the end
    }
    size64_t envc = 0;
    size64_t envs = 0;
    auto ev = envp;
    while(*ev){
        envc++;
        envs += strlen(*ev) + 1; // Add NULL char at the end
    }

    *Size = sizeof(void*) + (argc + 1) * sizeof(char*) + (envc + 1) * sizeof(char*) + args + envs;
    void* Buffer = (void*)malloc(*Size);
    
    void* StackDst = Buffer;

    *(void**)StackDst = (void*)argc;
    StackDst = (void*)((uint64_t)StackDst + sizeof(void*));

    void* OffsetDst = StackDst;
    StackDst = (void*)((uint64_t)StackDst + (argc + 1) * sizeof(char*) + (envc + 1) * sizeof(char*));

    for(int i = 0; i < argc; i++){
        *((void**)OffsetDst) = (void*)((uint64_t)StackDst - (uint64_t)Buffer);
        OffsetDst = (void*)((uint64_t)OffsetDst + sizeof(void*));
        strcpy((char*)StackDst, argv[i]);
        StackDst = (void*)((uint64_t)StackDst + strlen(argv[i]) + 1); // Add NULL char at the end
    }

    // Null argument
    *(void**)OffsetDst = NULL;
    OffsetDst = (void*)((uint64_t)OffsetDst + sizeof(void*));

    for(int i = 0; i < envc; i++){
        *(void**)OffsetDst = (void*)((uint64_t)StackDst - (uint64_t)Buffer);
        OffsetDst = (void*)((uint64_t)OffsetDst + sizeof(void*));
        strcpy((char*)StackDst, envp[i]);
        StackDst = (void*)((uint64_t)StackDst + strlen(envp[i]) + 1); // Add NULL char at the end
    }
    // Null argument
    *(void**)OffsetDst = NULL;

    *Data = Buffer;

    return KSUCCESS;
}

}

namespace mlibc{
    int sys_prepare_stack(void **stack, void *entry, void *user_arg, void *tcb, size_t *stack_size, size_t *guard_size) {
        // We can use this thread because stack is initialized in the same way for every thread
        kot_Sys_Thread_Info_Get(kot_Sys_GetThread(), 1, (uint64_t*)stack);
        kot_Sys_Thread_Info_Get(kot_Sys_GetThread(), 2, stack_size);
        *stack = (void*)((uint64_t)*stack - (uint64_t)*stack_size); // mlibc want the first address of the stack
        *guard_size = 0;
        return 0;
    }
}
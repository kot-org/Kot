#ifndef _GLOBAL_SCHEDULER_H
#define _GLOBAL_SCHEDULER_H 1

struct thread_t;
struct process_t;

#include <lib/lock.h>
#include <impl/vmm.h>
#include <global/mm.h>
#include <impl/arch.h>
#include <global/vfs.h>
#include <global/pmm.h>
#include <lib/vector.h>
#include <impl/context.h>
#include <sys/resource.h>
#include <global/resources.h>

#include <arch/include.h>
#include ARCH_INCLUDE(impl/scheduler.h)
/* 
The file in ARCH_INCLUDE(impl/scheduler.h) is expected to have :
    - The declaration of struct as type : thread_arch_state_t
*/

typedef uint64_t process_flags_t;

#define PROCESS_TYPE_EXEC       (0)
#define PROCESS_TYPE_MODULES    (1)
#define PROCESS_FLAG_TYPE_MASK  (1)

#define PROCESS_SET_FLAG_TYPE(type) (type & PROCESS_FLAG_TYPE_MASK)
#define PROCESS_GET_FLAG_TYPE(flags) (flags & PROCESS_FLAG_TYPE_MASK)

#define PROCESS_STACK_SIZE      0x400000

typedef struct thread_t{
    context_t* ctx;

    struct process_t* process; 
    void* entry_point; 

    void* stack;
    void* stack_base;
    size_t stack_size;

    int tid;

    thread_arch_state_t* arch_state;

    bool is_pausing;
    bool is_exiting;

    uint64_t index;

    struct thread_t* next;
    struct thread_t* last;

    bool is_in_queue;

    bool is_stack_free_disabled;
} thread_t;

typedef struct process_t{
    process_flags_t flags;
    context_flags_t ctx_flags;
    vfs_ctx_t* vfs_ctx;
    thread_t* entry_thread;
    spinlock_t data_lock;
    memory_handler_t* memory_handler;
    descriptors_ctx_t descriptors_ctx;

    vector_t* threads;

    pid_t pid;

    struct process_t* parent;
    
    vector_t* childs_result;

    int return_status;
} process_t;

extern process_t* proc_kernel;

void scheduler_init(void);

void scheduler_handler(cpu_context_t* ctx, uint8_t cpu_id);

void scheduler_generate_task_switching(void); /* note the following function should be define in arch folder */

process_t* scheduler_create_process(process_flags_t flags);
int scheduler_launch_process(process_t* process);
int scheduler_free_process(process_t* process);
int scheduler_exit_process(process_t* process, cpu_context_t* ctx);

thread_t* scheduler_create_thread(process_t* process, void* entry_point, void* stack, void* stack_base, size_t stack_size);
int scheduler_arch_prctl_thread(thread_t* thread, int code, void* address); /* note the following function should be define in arch folder */
int scheduler_launch_thread(thread_t* thread, arguments_t* args);
int scheduler_pause_thread(thread_t* thread, cpu_context_t* ctx);
int scheduler_unpause_thread(thread_t* thread);
int scheduler_free_thread(thread_t* thread);
int scheduler_exit_thread(thread_t* thread, cpu_context_t* ctx);
thread_t* scheduler_get_current_thread(void);
int scheduler_waitpid(pid_t pid, int* status, int flags, struct rusage* ru, cpu_context_t* ctx);
int scheduler_execve_syscall(char* path, int argc, char** args, char** envp, cpu_context_t* ctx);
void scheduler_fork_syscall(cpu_context_t* ctx);


#endif // _GLOBAL_SCHEDULER_H
#ifndef _GLOBAL_SCHEDULER_H
#define _GLOBAL_SCHEDULER_H 1

#include <impl/vmm.h>
#include <impl/arch.h>
#include <impl/context.h>

typedef uint64_t process_flags_t;

#define PROCESS_TYPE_APP        (0)
#define PROCESS_TYPE_MODULES    (1)
#define PROCESS_FLAG_TYPE_MASK  (1)

#define PROCESS_SET_FLAG_TYPE(type) (type & PROCESS_FLAG_TYPE_MASK)
#define PROCESS_GET_FLAG_TYPE(flags) (flags & PROCESS_FLAG_TYPE_MASK)

typedef struct thread_t{
    context_t* ctx;

    struct process_t* process; 
    void* entry_point; 
    void* stack;

    struct thread_t* next;
    struct thread_t* last;
} thread_t;

typedef struct process_t{
    process_flags_t flags;
    context_flags_t ctx_flags;
    vmm_space_t vmm_space;
} process_t;


void scheduler_handler(cpu_context_t* ctx);

process_t* scheduler_create_process(process_flags_t flags);

thread_t* scheduler_create_thread(process_t* process, void* entry_point, void* stack);

void scheduler_launcher_thread(thread_t* thread, arguments_t* args);

#endif // _GLOBAL_SCHEDULER_H
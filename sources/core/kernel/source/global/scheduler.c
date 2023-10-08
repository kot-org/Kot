#include <stddef.h>
#include <lib/log.h>
#include <stdbool.h>
#include <lib/lock.h>
#include <impl/arch.h>
#include <lib/assert.h>
#include <global/heap.h>
#include <impl/context.h>
#include <global/scheduler.h>

static spinlock_t scheduler_spinlock = {};

static thread_t* scheduler_first_node;
static thread_t* scheduler_last_node;

static pid_t scheduler_pid_iteration = 0;
static spinlock_t scheduler_pid_iteration_spinlock = {};

static int scheduler_tid_iteration = 0;
static spinlock_t scheduler_tid_iteration_spinlock = {};

process_t* proc_kernel = NULL;

static void scheduler_iddle(void){
    while(true){
        scheduler_generate_task_switching();
    }    
}

static void scheduler_enqueue_wl(thread_t* thread){
    if(scheduler_first_node == NULL){
        scheduler_first_node = thread;
    }

    if(scheduler_last_node != NULL){
        scheduler_last_node->next = thread;
        thread->last = scheduler_last_node;
    }

    scheduler_last_node = thread;
    thread->next = scheduler_first_node;
    thread->is_in_queue = true;
}

static void scheduler_dequeue_wl(thread_t* thread){
    if(scheduler_first_node == thread){
        if(scheduler_first_node != thread->next){
           scheduler_first_node = thread->next; 
        }else{
            scheduler_first_node = NULL;
            scheduler_last_node = NULL;
        }
    }
    if(scheduler_last_node == thread){
        if(thread->last != NULL){
            scheduler_last_node = thread->last; 
        }else{
            scheduler_last_node = scheduler_first_node;
        }
        
    }      

    if(scheduler_last_node != NULL){
        scheduler_last_node->next = scheduler_first_node;   
    }
   

    if(thread->last != NULL) thread->last->next = thread->next;
    thread->next->last = thread->last;
    thread->last = NULL;
    thread->next = NULL;
    thread->is_in_queue = false;
}

static thread_t* scheduler_get_tread_wl(void){
    thread_t* return_value = scheduler_first_node;
    if(return_value != NULL){
        scheduler_dequeue_wl(return_value);
    }
    return return_value;
}

static void scheduler_enqueue(thread_t* thread){
    spinlock_acquire(&scheduler_spinlock);
    scheduler_enqueue_wl(thread);
    spinlock_release(&scheduler_spinlock);
}

static void scheduler_dequeue(thread_t* thread){
    spinlock_acquire(&scheduler_spinlock);
    scheduler_dequeue_wl(thread);
    spinlock_release(&scheduler_spinlock);
}

static pid_t scheduler_get_new_pid(void){
    spinlock_acquire(&scheduler_pid_iteration_spinlock);

    pid_t pid = scheduler_pid_iteration;
    scheduler_pid_iteration++;

    spinlock_release(&scheduler_pid_iteration_spinlock);

    return pid;
}

static int scheduler_get_new_tid(void){
    spinlock_acquire(&scheduler_tid_iteration_spinlock);

    int tid = scheduler_tid_iteration;
    scheduler_tid_iteration++;

    spinlock_release(&scheduler_tid_iteration_spinlock);
    
    return tid;
}

void scheduler_init(void){
    proc_kernel = scheduler_create_process(PROCESS_TYPE_MODULES);
}

void scheduler_handler(cpu_context_t* ctx, uint8_t cpu_id){
    if(spinlock_test_and_acq(&scheduler_spinlock)){
        thread_t* ending_thread = ARCH_CONTEXT_CURRENT_THREAD(ctx);

        if(ending_thread != NULL){
            if(ending_thread->is_pausing){
                if(ending_thread->is_exiting){
                    scheduler_free_thread(ending_thread);
                }else{
                    context_save(ending_thread->ctx, ctx);
                }

                context_iddle(ctx, cpu_id);
            }else{
                context_save(ending_thread->ctx, ctx);
                scheduler_enqueue_wl(ending_thread);
            }
        }

        thread_t* starting_thread = scheduler_get_tread_wl();
        if(starting_thread != NULL){
            context_restore(starting_thread->ctx, ctx);
        }
        spinlock_release(&scheduler_spinlock);
    }
}

process_t* scheduler_create_process(process_flags_t flags){
    process_t* process = (process_t*)calloc(1, sizeof(process_t));

    process->ctx_flags = ((PROCESS_GET_FLAG_TYPE(flags) == PROCESS_TYPE_EXEC) ? CONTEXT_FLAG_USER : 0); 

    process->memory_handler = mm_create_handler(vmm_create_space(), (void*)VMM_USERSPACE_BOTTOM_ADDRESS, (size_t)((uintptr_t)VMM_USERSPACE_TOP_ADDRESS - (uintptr_t)VMM_USERSPACE_BOTTOM_ADDRESS));

    process->vfs_ctx = KERNEL_VFS_CTX;

    process->pid = scheduler_get_new_pid();

    process->threads = vector_create();

    return process;
}

int scheduler_launch_process(process_t* process){
    arguments_t args;

    args.arg[0] = (uintptr_t)process->entry_thread->stack;

    return scheduler_launch_thread(process->entry_thread, &args);
}

int scheduler_free_process(process_t* process){
    // TODO
    return 0;
}

int scheduler_exit_process(process_t* process, cpu_context_t* ctx){
    /* we do not free the current thread */
    int index = 0;
    while(process->threads->length > 1){
        thread_t* thread;
        do{
            thread = vector_get(process->threads, index);
            index++;
        }while(thread == ARCH_CONTEXT_CURRENT_THREAD(ctx));
        assert(!scheduler_exit_thread(thread, ctx));
    }

    assert(!scheduler_free_process(process));

    /* free maybe the current thread */
    assert(!scheduler_exit_thread(vector_get(process->threads, 0), ctx));

    return 0;
}

thread_t* scheduler_create_thread(process_t* process, void* entry_point, void* stack, void* stack_base, size_t stack_size){
    thread_t* thread = (thread_t*)calloc(1, sizeof(thread_t));

    thread->ctx = context_create();
    thread->stack = stack;
    thread->stack_base = stack_base;
    thread->stack_size = stack_size;
    thread->process = process;
    thread->entry_point = entry_point;

    thread->tid = scheduler_get_new_tid();

    thread->index = vector_push(process->threads, thread);
    
    return thread;
}

int scheduler_launch_thread(thread_t* thread, arguments_t* args){
    context_start(thread->ctx, thread->process->memory_handler->vmm_space, thread->entry_point, thread->stack, args, thread->process->ctx_flags, thread);

    scheduler_enqueue(thread);

    return 0;
}

int scheduler_pause_thread(thread_t* thread, cpu_context_t* ctx){
    spinlock_acquire(&scheduler_spinlock);

    if(thread->is_pausing || thread->is_exiting){
        spinlock_release(&scheduler_spinlock);
        return EINVAL;
    }

    if(thread->is_in_queue){
        scheduler_dequeue(thread);

        spinlock_release(&scheduler_spinlock);
    }else{
        thread->is_pausing = true;

        spinlock_release(&scheduler_spinlock);

        if(ctx == NULL){
            scheduler_generate_task_switching();
        }else if(ARCH_CONTEXT_CURRENT_THREAD(ctx) == thread){
            scheduler_generate_task_switching();
        }
    }
}

int scheduler_unpause_thread(thread_t* thread){
    spinlock_acquire(&scheduler_spinlock);

    if(thread->is_pausing){
        thread->is_pausing = false;

        spinlock_release(&scheduler_spinlock);

        scheduler_enqueue(thread);
        return 0;
    }else{
        return EINVAL;
    }
}

int scheduler_free_thread(thread_t* thread){
    assert(!mm_free_region(thread->process->memory_handler, thread->stack_base, thread->stack_size));

    context_free(thread->ctx);

    free(thread);

    return 0;
}

int scheduler_exit_thread(thread_t* thread, cpu_context_t* ctx){
    vector_remove(thread->process->threads, thread->index);

    spinlock_acquire(&scheduler_spinlock);

    if(thread->is_in_queue){
        scheduler_dequeue(thread);
        scheduler_free_thread(thread);

        spinlock_release(&scheduler_spinlock);
    }else{
        thread->is_pausing = true;
        thread->is_exiting = true;

        spinlock_release(&scheduler_spinlock);

        if(ctx == NULL){
            scheduler_generate_task_switching();
        }else if(ARCH_CONTEXT_CURRENT_THREAD(ctx) == thread){
            scheduler_generate_task_switching();
        }
    }

    return 0;
}

thread_t* scheduler_get_current_thread(void){
    return context_get_thread();
}
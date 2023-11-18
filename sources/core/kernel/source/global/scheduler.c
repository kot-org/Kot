#include <stddef.h>
#include <lib/log.h>
#include <stdbool.h>
#include <lib/lock.h>
#include <impl/arch.h>
#include <lib/assert.h>
#include <global/heap.h>
#include <impl/context.h>
#include <sys/resource.h>
#include <global/scheduler.h>
#include <global/elf_loader.h>

static spinlock_t scheduler_spinlock = SPINLOCK_INIT;

static thread_t* scheduler_first_node;
static thread_t* scheduler_last_node;

static pid_t scheduler_pid_iteration = 0;
static spinlock_t scheduler_pid_iteration_spinlock = SPINLOCK_INIT;

static int scheduler_tid_iteration = 0;
static spinlock_t scheduler_tid_iteration_spinlock = SPINLOCK_INIT;

static vector_t* schduler_waitpid_list = NULL;

struct child_result_info{
    pid_t return_pid;
    int return_status;
};

struct waitpid_info{
    thread_t* waiting_thread;
    pid_t pid;
    int flags;
    struct child_result_info child_result;
};

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

static void waitpid_return(pid_t pid, int status, struct waitpid_info* info, uint64_t index){
    info->waiting_thread->is_pausing = false;
    scheduler_enqueue_wl(info->waiting_thread);

    vector_remove(schduler_waitpid_list, index);
}

static void exit_thread_routine(thread_t* thread){
    if(thread == thread->process->entry_thread){
        if(thread->process->parent != NULL){
            for(uint64_t i = 0; i < schduler_waitpid_list->length; i++){
                struct waitpid_info* info = vector_get(schduler_waitpid_list, i);
                if(info->waiting_thread->process->pid == thread->process->parent->pid){
                    if(info->pid == -1){
                        waitpid_return(thread->process->pid, thread->process->return_status, info, i);
                    }
                }
            }
        }

        if(thread->process->parent != NULL){
            struct child_result_info* child_result = (struct child_result_info*)malloc(sizeof(struct child_result_info));
            child_result->return_pid = thread->process->pid;
            child_result->return_status = thread->process->return_status;
            vector_push(thread->process->parent->childs_result, child_result);
        }
    }

    scheduler_free_thread(thread);
}

void scheduler_init(void){
    proc_kernel = scheduler_create_process(PROCESS_TYPE_MODULES);
    schduler_waitpid_list = vector_create();
}

void scheduler_handler(cpu_context_t* ctx, uint8_t cpu_id){
    if(spinlock_test_and_acq(&scheduler_spinlock)){
        thread_t* ending_thread = ARCH_CONTEXT_CURRENT_THREAD(ctx);

        if(ending_thread != NULL){
            if(ending_thread->is_pausing){
                if(ending_thread->is_exiting){
                    exit_thread_routine(ending_thread);
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

    process->flags = flags;

    process->ctx_flags = ((PROCESS_GET_FLAG_TYPE(flags) == PROCESS_TYPE_EXEC) ? CONTEXT_FLAG_USER : 0); 

    process->memory_handler = mm_create_handler(vmm_create_space(), (void*)VMM_USERSPACE_BOTTOM_ADDRESS, (size_t)((uintptr_t)VMM_USERSPACE_TOP_ADDRESS - (uintptr_t)VMM_USERSPACE_BOTTOM_ADDRESS));

    process->vfs_ctx = KERNEL_VFS_CTX;

    process->pid = scheduler_get_new_pid();

    process->threads = vector_create();

    process->childs_result = vector_create();

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

    for(int i = 0; i < process->threads->length; i++){
        thread_t* thread = vector_get(process->threads, i);
        if(thread != ARCH_CONTEXT_CURRENT_THREAD(ctx) && thread != NULL){
            assert(!scheduler_exit_thread(thread, ctx));
        }
    }

    assert(!scheduler_free_process(process));

    /* free maybe the current thread */
    assert(!scheduler_exit_thread(ARCH_CONTEXT_CURRENT_THREAD(ctx), ctx));

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
    if(!thread->is_stack_free_disabled){
        assert(!mm_free_region(thread->process->memory_handler, thread->stack_base, thread->stack_size));
    }

    context_free(thread->ctx);

    free(thread);

    return 0;
}

int scheduler_exit_thread(thread_t* thread, cpu_context_t* ctx){
    spinlock_acquire(&scheduler_spinlock);
    
    vector_set(thread->process->threads, thread->index, NULL);

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

int scheduler_waitpid(pid_t pid, int* status, int flags, struct rusage* ru, cpu_context_t* ctx){
    // TODO : support ru and flags
    
    if(flags){
        log_warning("Waitpid : flag not implemented : %d\n", flags);
    }

    if(ru != NULL){
        log_warning("Waitpid : rusage not implemented\n");
    }

    spinlock_acquire(&scheduler_spinlock);

    vector_t* childs_result = ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->childs_result;
    
    for(uint64_t i = 0; i < childs_result->length; i++){
        struct child_result_info* child_result = vector_get(childs_result, i); 
        if(pid == -1){
            vector_remove(childs_result, i);
            spinlock_release(&scheduler_spinlock);
            *status = child_result->return_status;
            return child_result->return_pid;
        }
    }

    struct waitpid_info* info = malloc(sizeof(struct waitpid_info));
    info->waiting_thread = ARCH_CONTEXT_CURRENT_THREAD(ctx);
    info->pid = pid;
    info->flags = flags;
    info->child_result = (struct child_result_info){0, 0};

    vector_push(schduler_waitpid_list, info);
    ARCH_CONTEXT_CURRENT_THREAD(ctx)->is_pausing = true;
    spinlock_release(&scheduler_spinlock);
    scheduler_generate_task_switching();

    pid_t return_pid = info->child_result.return_pid;
    *status = info->child_result.return_status;
    free(info);
    return return_pid;

}

int scheduler_execve_syscall(char* path, int argc, char** args, char** envp, cpu_context_t* ctx){
    process_t* process = ARCH_CONTEXT_CURRENT_THREAD(ctx)->process;

    int error = 0;
    kernel_file_t* file = f_open(process->vfs_ctx, path, 0, 0, &error);

    if(error){
        return error;
    }


    thread_t* old_entry_thread = process->entry_thread;

    process->entry_thread = NULL;

    memory_handler_t* old_memory_handler = process->memory_handler;

    process->memory_handler = mm_create_handler(vmm_create_space(), (void*)VMM_USERSPACE_BOTTOM_ADDRESS, (size_t)((uintptr_t)VMM_USERSPACE_TOP_ADDRESS - (uintptr_t)VMM_USERSPACE_BOTTOM_ADDRESS));

    error = load_elf_exec_with_file(process, file, argc, args, envp);
    
    if(error){
        memory_handler_t* memory_handler_to_free = process->memory_handler;
        process->entry_thread = old_entry_thread;
        process->memory_handler = old_memory_handler;
        mm_free_handler(memory_handler_to_free);
        return error;
    }

    // TODO free the old_entry_thread
    mm_free_handler(old_memory_handler);

    assert(!scheduler_launch_process(process));
    
    // because the stack should be already free before
    ARCH_CONTEXT_CURRENT_THREAD(ctx)->is_stack_free_disabled = true;
    assert(!scheduler_exit_thread(ARCH_CONTEXT_CURRENT_THREAD(ctx), ctx));

    __builtin_trap();
}

void scheduler_fork_syscall(cpu_context_t* ctx){
    process_t* new_process = scheduler_create_process(ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->flags);
    new_process->parent = ARCH_CONTEXT_CURRENT_THREAD(ctx)->process;

    assert(!mm_fork(new_process->memory_handler, ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->memory_handler));

    new_process->vfs_ctx = vfs_copy_ctx(ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->vfs_ctx);

    // TODO : make sure we don't loose descriptors when exiting the process
    copy_process_descriptors(&new_process->descriptors_ctx, &ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx);

    new_process->entry_thread = scheduler_create_thread(new_process, ARCH_CONTEXT_IP(ctx), ARCH_CONTEXT_SP(ctx), ARCH_CONTEXT_CURRENT_THREAD(ctx)->stack_base, PROCESS_STACK_SIZE);
    
    contex_fork(new_process->entry_thread->ctx, ctx, new_process->entry_thread);
    ARCH_CONTEXT_RETURN(&new_process->entry_thread->ctx->cpu_ctx) = 0; 

    scheduler_enqueue(new_process->entry_thread);

    ARCH_CONTEXT_RETURN(ctx) = new_process->pid; 
}
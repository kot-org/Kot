#include <stddef.h>
#include <stdbool.h>
#include <lib/lock.h>
#include <impl/arch.h>
#include <global/heap.h>
#include <impl/context.h>
#include <global/scheduler.h>

static spinlock_t scheduler_spinlock = {};

static thread_t* scheduler_first_node;
static thread_t* scheduler_last_node;

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
}

static void scheduler_dequeue_wl(thread_t* thread){
    if(scheduler_first_node == thread){
        if(scheduler_first_node != thread->next){
           scheduler_first_node = thread->next; 
        }else{
            scheduler_first_node = NULL;
            scheduler_last_node = NULL;
            return;
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

void scheduler_handler(cpu_context_t* ctx){
    if(spinlock_test_and_acq(&scheduler_spinlock)){
        thread_t* ending_thread = ARCH_CONTEXT_SYSCALL_SELECTOR(ctx);

        if(ending_thread){
            context_save(ending_thread->ctx, ctx);
            scheduler_enqueue_wl(ending_thread);
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

    process->ctx_flags = ((PROCESS_GET_FLAG_TYPE(flags) == PROCESS_TYPE_APP) ? CONTEXT_FLAG_USER : 0); 

    process->vmm_space = vmm_create_space();

    return process;
}

thread_t* scheduler_create_thread(process_t* process, void* entry_point, void* stack){
    thread_t* thread = (thread_t*)calloc(1, sizeof(thread_t));

    thread->ctx = context_create();
    thread->stack = stack;
    thread->process = process;
    thread->entry_point = entry_point;

    return thread;
}

void scheduler_launcher_thread(thread_t* thread, arguments_t* args){
    context_start(thread->ctx, thread->process->vmm_space, thread->entry_point, thread->stack, args, thread->process->ctx_flags);

    scheduler_enqueue(thread);
}
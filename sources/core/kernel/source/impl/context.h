#ifndef _IMPL_CONTEXT_H
#define _IMPL_CONTEXT_H 1

#include <impl/vmm.h>
#include <impl/arch.h>
#include <lib/arguments.h>
#include <global/scheduler.h>

#include <arch/include.h>
/* 
The file in ARCH_INCLUDE(impl/context.h) is expected to have :
    - The declaration of type : context_t
*/
#include ARCH_INCLUDE(impl/context.h)

#define CONTEXT_FLAG_USER (1 << 0)

typedef uint64_t context_flags_t;

context_t* context_create(void);

void context_free(context_t* ctx);

void context_start(context_t* ctx, vmm_space_t vmm_space, void* ip, void* sp, arguments_t* args, context_flags_t flags, struct thread_t* thread);

void context_save(context_t* ctx, cpu_context_t* cpu_ctx);

void context_restore(context_t* ctx, cpu_context_t* cpu_ctx);

void contex_fork(context_t* ctx, cpu_context_t* cpu_ctx, struct thread_t* thread);

void context_signal(struct signal_handler_t* handler, struct thread_t* thread, int signal);

void* context_get_thread(void);

#endif // _IMPL_CONTEXT_H
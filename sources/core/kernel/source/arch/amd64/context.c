#include <impl/vmm.h>
#include <impl/arch.h>
#include <lib/memory.h>
#include <global/heap.h>

#include <arch/include.h>
#include ARCH_INCLUDE(gdt.h)
#include ARCH_INCLUDE(cpu.h)
#include ARCH_INCLUDE(simd.h)
#include ARCH_INCLUDE(context.h)

context_info_t* iddle_ctx_info[MAX_CORE_COUNT];

static void context_iddle_handler(void){
    while(true){
        asm("pause");
    }
}

void context_init(uint8_t cpu_count){
    for(uint8_t i = 0; i < cpu_count; i++){
        iddle_ctx_info[i] = calloc(1, sizeof(context_info_t));
        iddle_ctx_info[i]->cs = GDT_KERNEL_CODE * sizeof(gdt_entry_t);
        iddle_ctx_info[i]->ss = GDT_KERNEL_DATA * sizeof(gdt_entry_t);
        iddle_ctx_info[i]->kernel_stack = malloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
    }
}

context_t* context_create(void){
    context_t* ctx = calloc(1, sizeof(context_t));
    ctx->simd_ctx = simd_create_context();
    return ctx;
}

void context_free(context_t* ctx){
    simd_free_context(ctx->simd_ctx);
    free(ctx);
}

void context_start(context_t* ctx, vmm_space_t vmm_space, void* ip, void* sp, arguments_t* args, context_flags_t flags, thread_t* thread){
    ctx->cpu_ctx.cr3 = (uint64_t)vmm_space; 
    ctx->cpu_ctx.rip = (uint64_t)ip; 
    ctx->cpu_ctx.rsp = (uint64_t)sp; 
    ctx->cpu_ctx.rflags = RFLAGS_INTERRUPT_ENABLE | RFLAGS_ONE;
    
    if(args != NULL){
        ARCH_CONTEXT_ARG0(&ctx->cpu_ctx) = args->arg[0];
        ARCH_CONTEXT_ARG1(&ctx->cpu_ctx) = args->arg[1];
        ARCH_CONTEXT_ARG2(&ctx->cpu_ctx) = args->arg[2];
        ARCH_CONTEXT_ARG3(&ctx->cpu_ctx) = args->arg[3];
        ARCH_CONTEXT_ARG4(&ctx->cpu_ctx) = args->arg[4];
        ARCH_CONTEXT_ARG5(&ctx->cpu_ctx) = args->arg[5];
    }

    if(flags & CONTEXT_FLAG_USER){
        ctx->cpu_ctx.cs = GDT_USER_CODE * sizeof(gdt_entry_t) | GDT_RING_3;
        ctx->cpu_ctx.ss = GDT_USER_DATA * sizeof(gdt_entry_t) | GDT_RING_3;
    }else{
        ctx->cpu_ctx.cs = GDT_KERNEL_CODE * sizeof(gdt_entry_t);
        ctx->cpu_ctx.ss = GDT_KERNEL_DATA * sizeof(gdt_entry_t);
    }

    ctx->cpu_ctx.ctx_info = (context_info_t*)calloc(1, sizeof(context_info_t));
    ctx->cpu_ctx.ctx_info->thread = thread;
    ctx->cpu_ctx.ctx_info->cs = ctx->cpu_ctx.cs;
    ctx->cpu_ctx.ctx_info->ss = ctx->cpu_ctx.ss;
    ctx->cpu_ctx.ctx_info->kernel_stack = malloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
}

void context_save(context_t* ctx, cpu_context_t* cpu_ctx){
    simd_save_context(ctx->simd_ctx);
    memcpy(&ctx->cpu_ctx, cpu_ctx, sizeof(cpu_context_t));
}

void context_restore(context_t* ctx, cpu_context_t* cpu_ctx){
    simd_restore_context(ctx->simd_ctx);
    set_cpu_fs_base(ctx->fs_base);
    memcpy(cpu_ctx, &ctx->cpu_ctx, sizeof(cpu_context_t));
}

void contex_fork(context_t* ctx, cpu_context_t* cpu_ctx, thread_t* thread){
    context_save(ctx, cpu_ctx);
    ctx->fs_base = ((thread_t*)cpu_ctx->ctx_info->thread)->ctx->fs_base;
    ctx->cpu_ctx.cr3 = (uint64_t)thread->process->memory_handler->vmm_space;
    ctx->cpu_ctx.ctx_info = (context_info_t*)calloc(1, sizeof(context_info_t));
    ctx->cpu_ctx.ctx_info->thread = thread;
    ctx->cpu_ctx.ctx_info->cs = ctx->cpu_ctx.cs;
    ctx->cpu_ctx.ctx_info->ss = ctx->cpu_ctx.ss;
    ctx->cpu_ctx.ctx_info->kernel_stack = malloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
}

void context_iddle(cpu_context_t* cpu_ctx, uint8_t cpu_id){
    cpu_ctx->ctx_info = iddle_ctx_info[cpu_id];
    cpu_ctx->cr3 = (uint64_t)vmm_get_kernel_space(); 
    cpu_ctx->rip = (uint64_t)&context_iddle_handler; 
    cpu_ctx->rsp = (uint64_t)iddle_ctx_info[cpu_id]->kernel_stack; 
    cpu_ctx->cs = cpu_ctx->ctx_info->cs;
    cpu_ctx->ss = cpu_ctx->ctx_info->ss;
    cpu_ctx->rflags = RFLAGS_INTERRUPT_ENABLE | RFLAGS_ONE;
}

void* context_get_thread(void){
    context_info_t* ctx_info = NULL;
    asm("mov %%gs:0x8, %0":"=r"(ctx_info));
    return ctx_info->thread;
}
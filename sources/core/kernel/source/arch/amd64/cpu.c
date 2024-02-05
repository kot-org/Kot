#include <stdint.h>
#include <impl/arch.h>
#include <global/heap.h>

#include <arch/include.h>
#include ARCH_INCLUDE(cpu.h)
#include ARCH_INCLUDE(gdt.h)

struct cpu_context{
    uint64_t id;
    context_info_t* ctx_info;
    uint64_t user_stack;
    tss_t* cpu_tss;
}__attribute__((packed));

static void cpu_init_tss(struct cpu_context* cpu_ctx) {
    cpu_ctx->cpu_tss = (tss_t*)calloc(1, sizeof(tss_t));
    cpu_ctx->cpu_tss->rsp[0] = (uint64_t)((uintptr_t)malloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE);
    gdt_load_tss(cpu_ctx->cpu_tss);
}


void cpu_init(void) {
    struct cpu_context* cpu_ctx = (struct cpu_context*)calloc(1, sizeof(struct cpu_context));
    cpu_ctx->id = cpu_get_apicid();
    cpu_ctx->ctx_info = (context_info_t*)calloc(1, sizeof(context_info_t));
    reload_gs_fs();
    set_cpu_gs_base((uint64_t)cpu_ctx);
    cpu_init_tss(cpu_ctx);
}
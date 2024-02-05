#include <asm/prctl.h>
#include <global/scheduler.h>

#include <arch/include.h>
#include ARCH_INCLUDE(cpu.h)
#include ARCH_INCLUDE(context.h)
#include ARCH_INCLUDE(interrupts.h)

void scheduler_generate_task_switching(void){
    asm("int %0" : : "i"(INT_SCHEDULE));
}

int scheduler_arch_prctl_thread(thread_t* thread, int code, void* address){
    switch(code){
        case ARCH_GET_FS:{
            if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){address, sizeof(uint64_t)})){
                return EINVAL;
            }
            *(uint64_t*)address = thread->ctx->fs_base;
            break;
        }  
        case ARCH_SET_FS:{
            thread->ctx->fs_base = (uint64_t)address;
            thread->signal_restore_ctx->fs_base = (uint64_t)address;
            set_cpu_fs_base((uint64_t)address);
            break;
        }
        default:{
            return EINVAL;
        }
    }
    return 0;
}
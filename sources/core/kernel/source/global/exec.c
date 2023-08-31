#include <errno.h>
#include <lib/assert.h>
#include <global/exec.h>
#include <global/scheduler.h>
#include <global/elf_loader.h>

int create_exec(process_t* parent, int argc, char* args[], char* envp[]){
    process_t* process_ctx = scheduler_create_process(PROCESS_SET_FLAG_TYPE(PROCESS_TYPE_EXEC));

    process_ctx->vfs_ctx = vfs_copy_ctx(parent->vfs_ctx);

    void* stack;
    assert(!mm_allocate_region_vm(process_ctx->memory_handler, NULL, PROCESS_STACK_SIZE, false, &stack));

    int err = load_elf_exec(process_ctx, argc, args, envp, stack);

    if(err){
        assert(!mm_free_region(process_ctx->memory_handler, stack, PROCESS_STACK_SIZE));
        vfs_free_ctx(process_ctx->vfs_ctx);
        scheduler_free_process(process_ctx);

        return err;
    }

    return 0;
}

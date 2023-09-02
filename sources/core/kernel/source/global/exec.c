#include <errno.h>
#include <lib/assert.h>
#include <global/exec.h>
#include <global/scheduler.h>
#include <global/elf_loader.h>

int create_exec(process_t* parent, int argc, char* args[], char* envp[]){
    process_t* process_ctx = scheduler_create_process(PROCESS_SET_FLAG_TYPE(PROCESS_TYPE_EXEC));

    process_ctx->vfs_ctx = vfs_copy_ctx(parent->vfs_ctx);

    int err = load_elf_exec(process_ctx, argc, args, envp);

    if(err){
        vfs_free_ctx(process_ctx->vfs_ctx);
        scheduler_free_process(process_ctx);

        return err;
    }

    err = scheduler_launch_process(process_ctx);

    if(err){
        vfs_free_ctx(process_ctx->vfs_ctx);
        scheduler_free_process(process_ctx);

        return err;
    }
    
    return 0;
}

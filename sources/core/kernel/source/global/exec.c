#include <errno.h>
#include <lib/assert.h>
#include <global/heap.h>
#include <global/scheduler.h>
#include <global/elf_loader.h>

static inline int create_std_file_descriptor(process_t* process_ctx){
    int error = 0;

    for(int i = 0; i < 3; i++){
        kernel_file_t* std_file = f_open(process_ctx->vfs_ctx, "dev/tty0", 0, 0, &error);
        assert(!error);
        
        descriptor_t* descriptor = malloc(sizeof(descriptor_t));
        
        descriptor->type = DESCRIPTOR_TYPE_FILE;
        descriptor->data.file = std_file;

        assert(add_descriptor(&process_ctx->descriptors_ctx, descriptor) == i);  
    }

    return 0;
}

int create_exec(process_t* parent, char* path, int argc, char* args[], char* envp[]){
    process_t* process_ctx = scheduler_create_process(PROCESS_SET_FLAG_TYPE(PROCESS_TYPE_EXEC));

    process_ctx->vfs_ctx = vfs_copy_ctx(parent->vfs_ctx);


    int err = load_elf_exec(process_ctx, path, argc, args, envp);

    if(err){
        vfs_free_ctx(process_ctx->vfs_ctx);
        scheduler_free_process(process_ctx);

        return err;
    }

    assert(!create_std_file_descriptor(process_ctx));

    assert(!scheduler_launch_process(process_ctx));
    
    return 0;
}

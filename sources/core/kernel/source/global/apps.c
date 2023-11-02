#include <lib/string.h>
#include <impl/panic.h>
#include <global/exec.h>

static const char* bin_path = "usr/bin/";
static const char* app_init_path = "/sda/system/init/init.elf";


void apps_init(void){
    char* args[1];
    args[0] = (char*)app_init_path;

    char* envp[1];
    envp[0] = NULL;

    proc_kernel->vfs_ctx->cwd = (char*)bin_path;
    proc_kernel->vfs_ctx->cwd_size = strlen(bin_path);

    int error = create_exec(proc_kernel, args[0], 1, args, envp);

    if(error){
        panic("Initialisation app: %s can't be load correctly !! \nError code : %d", app_init_path, error);
    }
}
#include <impl/panic.h>
#include <global/exec.h>

static const char* app_init_path = "/sda/bin/sh";

void apps_init(void){
    char* args[1];
    args[0] = (char*)app_init_path;

    char* envp[1];
    envp[0] = NULL;

    int error = create_exec(proc_kernel, 1, args, envp);

    if(error){
        panic("Initialisation app: %s can't be load correctly !! \nError code : %d", app_init_path, error);
    }
}
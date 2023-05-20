#include <stdio.h>
#include <kot/shell.h>
#include <bits/ensure.h>
#include <frg/string.hpp>
#include <frg/vector.hpp>
#include <mlibc/debug.hpp>
#include <kot/uisd/srvs/storage.h>

int kot_InitializeShell(){
    kot_uisd_shell_t* Shell = (kot_uisd_shell_t*)kot_FindControllerUISD(ControllerTypeEnum_Shell);
    if(!Shell->IsAvailableAsFile){
        mlibc::infoLogger() << "mlibc warning: Shell isn't available as file" << frg::endlog;
        mlibc::infoLogger() << "mlibc warning: Shell initialization failed" << frg::endlog;
        return -1;
    }
    __ensure(kot_OpenShellFile("/dev/tty") == 0);
    return 0;
}
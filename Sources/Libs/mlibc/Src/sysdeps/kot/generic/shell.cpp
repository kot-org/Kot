#include <stdio.h>
#include <kot/shell.h>
#include <bits/ensure.h>
#include <frg/string.hpp>
#include <frg/vector.hpp>
#include <mlibc/debug.hpp>
#include <kot/uisd/srvs/storage.h>

namespace Kot{
    int InitializeShell(){
        uisd_shell_t* Shell = (uisd_shell_t*)Kot::FindControllerUISD(ControllerTypeEnum_Shell);
        if(!Shell->IsAvailableAsFile){
            mlibc::infoLogger() << "mlibc warning: Shell isn't available as file" << frg::endlog;
            mlibc::infoLogger() << "mlibc warning: Shell initialization failed" << frg::endlog;
            return -1;
        }
        __ensure(OpenShellFile("/dev/tty") == 0);
        return 0;
    }

}
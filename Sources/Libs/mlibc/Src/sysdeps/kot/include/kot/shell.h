#ifndef _MEMORY_H
#define _MEMORY_H 1

#include <kot/sys.h>
#include <kot/uisd.h>
#include <kot/types.h>
#include <kot/atomic.h>

namespace Kot{
    int InitializeShell();
    int OpenShellFile(const char *pathname);
}


#endif
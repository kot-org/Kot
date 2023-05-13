#ifndef KOT_THREAD_H
#define KOT_THREAD_H

#include <stdint.h>
#include <kot/sys.h>
#include <kot/types.h>

namespace Kot{
    KResult InitializeThread(kot_thread_t thread);
    KResult SetupStack(uintptr_t* Data, size64_t* Size, int argc, char** argv, char** envp);
}

#endif
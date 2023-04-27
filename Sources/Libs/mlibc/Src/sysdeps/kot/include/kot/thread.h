#ifndef KOT_THREAD_H
#define KOT_THREAD_H

#include <stdint.h>
#include <kot/sys.h>
#include <kot/types.h>

namespace Kot{
    KResult InitializeThread(kot_thread_t thread);
}

#endif
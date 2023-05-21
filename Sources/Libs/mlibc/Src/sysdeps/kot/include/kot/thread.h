#ifndef KOT_THREAD_H
#define KOT_THREAD_H

#include <stdint.h>
#include <kot/sys.h>
#include <kot/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

KResult kot_InitializeThread(kot_thread_t thread);
KResult kot_SetupStack(uintptr_t* Data, size64_t* Size, int argc, char** argv, char** envp);

#if defined(__cplusplus)
} 
#endif

#endif
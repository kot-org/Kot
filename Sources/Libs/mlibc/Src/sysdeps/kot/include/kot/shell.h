#ifndef _MEMORY_H
#define _MEMORY_H 1

#include <kot/sys.h>
#include <kot/uisd.h>
#include <kot/types.h>
#include <kot/atomic.h>

#if defined(__cplusplus)
extern "C" {
#endif

int kot_InitializeShell();
int kot_OpenShellFile(const char *pathname);


#if defined(__cplusplus)
} 
#endif

#endif
#ifndef KOT_LAUNCH_H
#define KOT_LAUNCH_H 1

#include <kot/sys.h>
#include <kot/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

KResult kot_launch(const char *path, char *const argv[], char *const envp[]);

#if defined(__cplusplus)
}
#endif

#endif

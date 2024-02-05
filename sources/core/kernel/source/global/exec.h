#ifndef _GLOBAL_EXEC_H
#define _GLOBAL_EXEC_H 1

#include <stdint.h>
#include <stddef.h>

#include <global/scheduler.h>

int create_exec(process_t* parent, char* path, int argc, char* args[], char* envp[]);

#endif // _GLOBAL_EXEC_H
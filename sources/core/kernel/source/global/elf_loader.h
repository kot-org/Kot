#ifndef _GLOBAL_ELF_LOADER_H
#define _GLOBAL_ELF_LOADER_H 1

#include <global/modules.h>
#include <global/scheduler.h>

int load_elf_module(module_metadata_t** metadata, int argc, char* args[]);
int load_elf_exec(process_t* process_ctx, int argc, char* args[], char* envp[]);

#endif // _GLOBAL_ELF_LOADER_H
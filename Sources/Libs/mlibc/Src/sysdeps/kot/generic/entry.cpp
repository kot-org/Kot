#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <kot/shell.h>
#include <bits/ensure.h>
#include <frg/string.hpp>
#include <frg/vector.hpp>
#include <mlibc/debug.hpp>
#include <kot/descriptor.h>
#include <mlibc/elf/startup.h>

#if MLIBC_STATIC_BUILD
void* __dso_handle;
#endif

// defined by the POSIX library
void __mlibc_initLocale();

extern "C" uintptr_t *__dlapi_entrystack();
extern "C" void __dlapi_enter(uintptr_t *);

extern char **environ;
mlibc::exec_stack_data __mlibc_stack_data;

struct LibraryGuard {
	LibraryGuard();
};

uint64_t ExecFlags = 0;

static LibraryGuard guard;

LibraryGuard::LibraryGuard() {
	__mlibc_initLocale();

	mlibc::parse_exec_stack(__dlapi_entrystack(), &__mlibc_stack_data);

	// Convert offset to pointer
	char** argv = __mlibc_stack_data.argv;
	for(int i = 0; i < __mlibc_stack_data.argc; i++){
        *argv = (char*)((uint64_t)(*argv) + (uint64_t)__dlapi_entrystack());
		argv++;
    }

	char** ev = __mlibc_stack_data.envp;
	while(*ev){
		*ev = (char*)((uint64_t)(*ev) + (uint64_t)__dlapi_entrystack());
		ev++;
	}

	mlibc::set_startup_data(__mlibc_stack_data.argc, __mlibc_stack_data.argv,
			__mlibc_stack_data.envp);
}

extern int kot_InitFS(uint64_t Flags, int DescriptorInitializationReturnValue);

extern "C" void __mlibc_entry(uintptr_t *entry_stack, int (*main_fn)(int argc, char *argv[], char *env[]), uint64_t Flags) {
	int DescriptorInitialization = kot_InitializeDescriptorSaver();

	__dlapi_enter(entry_stack);

	ExecFlags = Flags;


	if(DescriptorInitialization < 0){
		mlibc::panicLogger() << "mlibc: descriptor can't be loaded" << frg::endlog;
	}

	kot_InitFS(Flags, DescriptorInitialization);
	if(!(Flags & EXEC_FLAGS_SHELL_DISABLED)){
		kot_InitializeShell();
	}
	auto result = main_fn(__mlibc_stack_data.argc, __mlibc_stack_data.argv, environ);
	exit(result);
}

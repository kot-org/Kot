#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <bits/ensure.h>
#include <frg/string.hpp>
#include <frg/vector.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/elf/startup.h>

#if MLIBC_STATIC_BUILD
void* __dso_handle;
#endif

// defined by the POSIX library
void __mlibc_initLocale();

extern "C" uintptr_t *__dlapi_entrystack();
extern "C" void __dlapi_enter(uintptr_t *);

extern char **environ;
static mlibc::exec_stack_data __mlibc_stack_data;

struct LibraryGuard {
	LibraryGuard();
};


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

extern "C" void __mlibc_entry(uintptr_t *entry_stack, int (*main_fn)(int argc, char *argv[], char *env[])) {
	__dlapi_enter(entry_stack);
	// open terminal
	stdin = fopen("d0:tty", "r");
	// stdout = fopen("d0:tty", "w");
	// stderr = fopen("d0:tty", "w+");
	auto result = main_fn(__mlibc_stack_data.argc, __mlibc_stack_data.argv, environ);
	exit(result);
}

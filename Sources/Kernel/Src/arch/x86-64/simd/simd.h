#include <arch/arch.h>

void simdInit();

uintptr_t simdCreateSaveSpace();
void simdSave(uintptr_t location);
void simdRestore(uintptr_t location);
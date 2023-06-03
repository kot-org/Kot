#include <arch/arch.h>

void simdInit();

void* simdCreateSaveSpace();
void simdSave(void* location);
void simdRestore(void* location);
#include <arch/arch.h>

void simdInit();

void* simdCreatSaveSpace();
void simdSave(void* location);
void simdRestore(void* location);
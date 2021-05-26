#include <stdint.h>
#include <stddef.h>
#include "../../UEFI/efi.h"
#include "../../UEFI/bootinfo.h"

extern void main(BootInfo* bootInfo);

void _start(BootInfo* bootInfo)
{
    main(bootInfo);
    return;
}
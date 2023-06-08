#pragma once

#include <settings.h>
#include <arch/arch.h>
#include <lib/types.h>
#include <memory/memory.h>
#include <lib/stdio/cstr.h>
#include <misc/bitmap/bitmap.h>
#include <misc/panic/panic.h>
#include <drivers/acpi/acpi.h>
#include <drivers/hpet/hpet.h>
#include <heap/heap.h>
#include <lib/limits.h>
#include <syscall/syscall.h>
#include <boot/ukl.h>
#include <logs/logs.h>
#include <fs/initrd.h>
#include <elf/elf.h>

extern "C" void main(ukl_boot_structure_t* BootData);
#pragma once



#include <arch/arch.h>
#include <kot/types.h>
#include <memory/memory.h>
#include <lib/stdio/cstr.h>
#include <misc/bitmap/bitmap.h>
#include <drivers/acpi/acpi.h>
#include <drivers/hpet/hpet.h>
#include <heap/heap.h>
#include <lib/limits.h>
#include <drivers/rtc/rtc.h>
#include <syscall/syscall.h>
#include <boot/boot.h>
#include <logs/logs.h>
#include <fs/ramfs.h>
#include <elf/elf.h>

extern "C" void main(uintptr_t boot);
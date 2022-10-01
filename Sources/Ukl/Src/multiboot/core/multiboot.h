#pragma once

#include <ukl.h>
#include <elf/elf.h>
#include <lib/cstring.h>
#include <multiboot/boot/boot.h>
#include <multiboot/memory/memory.h>
#include <multiboot/core/multiboot2.h>

void Print(const char* chr);
char* itoa(int32_t n, char* buffer, int basenumber);
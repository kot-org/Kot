#pragma once

#include <lib/sys.h>
#include <arch/arch.h>
#include <lib/types.h>
#include <lib/stack/stack.h>
#include <scheduler/scheduler.h>

void memset(void* start, uint8_t value, size64_t size);
void memset16(void* start, uint16_t value, size64_t size);
void memset32(void* start, uint32_t value, size64_t size);
void memset64(void* start, uint64_t value, size64_t size);

int memcmp(const void *aptr, const void *bptr, size64_t size);
void memcpy(void* destination, void* source, size64_t size);

bool CheckAddress(void* address, size64_t size, void* pagingEntry);
bool CheckAddress(void* address, size64_t size);

bool CheckUserAddress(void* address, size64_t size, void* pagingEntry);
bool CheckUserAddress(void* address, size64_t size);
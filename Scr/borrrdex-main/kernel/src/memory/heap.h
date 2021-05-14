#pragma once

#include <cstdint>
#include <cstddef>

void heap_init(void* virtualAddress, size_t numPages);
void heap_expand(size_t length);

void* kmalloc(size_t size);
void* krealloc(void* ptr, size_t size);
void* kcalloc(size_t, size_t);
void kfree(void* address);
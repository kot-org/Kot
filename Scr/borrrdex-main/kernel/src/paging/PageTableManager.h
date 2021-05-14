#pragma once

#include "Paging.h"

class Framebuffer;

constexpr uint64_t USERLAND_STACK_TOP = 0xFFFFFFFFFFFFFFFF;

class PageTableManager {
public:
    static void SetSystemMemorySize(uint64_t bytes);
    static void SetFramebuffer(Framebuffer* buffer);

    PageTableManager(PageTable* PML4Address);
    
    void MapMemory(void* virtualMemory, void* physicalMemory, bool forUser);
    void WriteToCR3();

    void InvalidatePage(uint64_t virtualAddress);
    const void* PML4Address() const { return _pml4; }

private:
    PageTable* _pml4;
};

void pagetable_setcr3(uint64_t address);
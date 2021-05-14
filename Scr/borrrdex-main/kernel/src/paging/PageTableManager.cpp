#include "PageTableManager.h"
#include "PageMapIndexer.h"
#include "PageFrameAllocator.h"
#include "string.h"
#include "KernelUtil.h"

#include <cstdint>
#include <utility>

static uint64_t s_system_memory_size;
static Framebuffer* s_framebuffer;

void pagetable_setcr3(uint64_t address) {
    asm ("mov %%rax, %%cr3" : : "a" (address));
}

void PageTableManager::SetSystemMemorySize(uint64_t bytes) {
    s_system_memory_size = bytes;
}

void PageTableManager::SetFramebuffer(Framebuffer* buffer) {
    s_framebuffer = buffer;
}

PageTableManager::PageTableManager(PageTable* PML4Address)
    :_pml4(PML4Address)
{
    for(uint64_t i = 0; i < s_system_memory_size; i += 0x1000) {
        MapMemory((void *)i, (void *)i, false);
    }

    if(s_framebuffer) {
        for(uint64_t i = 0; i < s_framebuffer->bufferSize; i += 0x1000) {
            MapMemory((void *)((uint64_t)s_framebuffer->baseAddress + i), (void *)((uint64_t)s_framebuffer->baseAddress + i), false);
        }
    }
}

void PageTableManager::MapMemory(void* virtualMemory, void* physicalMemory, bool forUser) {
    PageMapIndexer indexer((uint64_t)virtualMemory);
    PageDirectoryEntry pde = _pml4->entries[indexer.GetPDP()];

    PageTable* pdp;
    if(!pde.GetFlag(PT_Flag::Present)) {
        pdp = (PageTable *)PageFrameAllocator::SharedAllocator()->RequestPage();
        memset(pdp, 0, 0x1000);
        pde.SetAddress((uint64_t)pdp >> 12);
        pde.SetFlag(PT_Flag::Present, true);
        pde.SetFlag(PT_Flag::ReadWrite, true);
        pde.SetFlag(PT_Flag::UserSuper, forUser);
        _pml4->entries[indexer.GetPDP()] = pde;
    } else {
        if(forUser) {
            pde.SetFlag(PT_Flag::UserSuper, true);
            _pml4->entries[indexer.GetPDP()] = pde;
        }

        pdp = (PageTable *)((uint64_t)pde.GetAddress() << 12);
    }

    pde = pdp->entries[indexer.GetPD()];
    PageTable* pd;
    if(!pde.GetFlag(PT_Flag::Present)) {
        pd = (PageTable *)PageFrameAllocator::SharedAllocator()->RequestPage();
        memset(pd, 0, 0x1000);
        pde.SetAddress((uint64_t)pd >> 12);
        pde.SetFlag(PT_Flag::Present, true);
        pde.SetFlag(PT_Flag::ReadWrite, true);
        pde.SetFlag(PT_Flag::UserSuper, forUser);
        pdp->entries[indexer.GetPD()] = pde;
    } else {
        if(forUser) {
            pde.SetFlag(PT_Flag::UserSuper, true);
            pdp->entries[indexer.GetPD()] = pde;
        }

        pd = (PageTable *)((uint64_t)pde.GetAddress() << 12);
    }

    pde = pd->entries[indexer.GetPT()];
    PageTable* pt;
    if(!pde.GetFlag(PT_Flag::Present)) {
        pt = (PageTable *)PageFrameAllocator::SharedAllocator()->RequestPage();
        memset(pt, 0, 0x1000);
        pde.SetAddress((uint64_t)pt >> 12);
        pde.SetFlag(PT_Flag::Present, true);
        pde.SetFlag(PT_Flag::ReadWrite, true);
        pde.SetFlag(PT_Flag::UserSuper, forUser);
        pd->entries[indexer.GetPT()] = pde;
    } else {
        if(forUser) {
            pde.SetFlag(PT_Flag::UserSuper, true);
            pd->entries[indexer.GetPT()] = pde;
        }

        pt = (PageTable *)((uint64_t)pde.GetAddress() << 12);
    }

    pde = pt->entries[indexer.GetP()];
    pde.SetAddress((uint64_t)physicalMemory >> 12);
    pde.SetFlag(PT_Flag::Present, true);
    pde.SetFlag(PT_Flag::ReadWrite, true);
    pde.SetFlag(PT_Flag::UserSuper, forUser);
    pt->entries[indexer.GetP()] = pde;
}

void PageTableManager::WriteToCR3() {
    pagetable_setcr3((uint64_t)_pml4);
}

void PageTableManager::InvalidatePage(uint64_t virtualAddress) {
    asm volatile("invlpg (%%rax)" : : "a"(virtualAddress));
}
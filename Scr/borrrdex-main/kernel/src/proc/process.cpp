#include "process.h"
#include "thread.h"
#include "fs/vfs.h"
#include "proc/elf.h"
#include "paging/PageFrameAllocator.h"
#include "string.h"
#include "config.h"
#include "Panic.h"
#include "KernelUtil.h"
#include "scheduler.h"
#include "graphics/BasicRenderer.h"
#include "arch/x86_64/interrupt/interrupt.h"
#include "drivers/x86_64/arch.h"

static void process_set_pagetable(void* pagetable) {
    WithInterrupts wi(false);
    pagetable_setcr3((uint64_t)pagetable);
    thread_get_current_thread_entry()->context->pml4 = (uint64_t)pagetable;
    thread_get_current_thread_entry()->context->virt_memory = (uint64_t *)pagetable;
}

static int setup_new_process(tid_t thread, const char* executable, const char**argv_src,
    uint64_t* entry_point, uint64_t* stack_top) {
    tid_t current_thread = thread_get_current();
    thread_table_t* thread_entry = thread_get_thread_entry(thread);
    void* current_pt = thread_get_thread_entry(current_thread)->context->virt_memory;
    void* new_pt = PageFrameAllocator::SharedAllocator()->RequestPage();
    memcpy(new_pt, current_pt, PAGE_SIZE);

    openfile_t file = vfs_open(executable);
    if(file < 0) {
        return -1;
    }

    int elf_result;
    elf_info_t* elf = elf_info_create(file, &elf_result);
    if(!elf) {
        return elf_result;
    }

    *entry_point = elf->entry_point;
    void* phys_page = nullptr, *virt_page = nullptr;

    WithInterrupts wi(false);
    process_set_pagetable(new_pt);
    thread_entry->pml4 = new_pt;
    PageTableManager ptm((PageTable *)new_pt);
    VirtualFilesystemFile vf(file, true);
    for(int i = 0; i < CONFIG_USERLAND_STACK_SIZE; i++) {
        phys_page = PageFrameAllocator::SharedAllocator()->RequestPage();
        KERNEL_ASSERT(phys_page != nullptr);
        virt_page = (void *)((USERLAND_STACK_TOP & PAGE_SIZE_MASK) - i*PAGE_SIZE);
        ptm.MapMemory(virt_page, phys_page, true);
        memset(virt_page, 0, PAGE_SIZE);
    }

    for(int i = 0; i < elf->loadables_count; i++) {
        auto* nextLoadable = &elf->loadables[i];
        for(int pg = 0; pg < nextLoadable->pages; pg++) {
            int left_to_read = nextLoadable->size - pg * PAGE_SIZE;
            phys_page = PageFrameAllocator::SharedAllocator()->RequestPage();
            KERNEL_ASSERT(phys_page != nullptr);
            virt_page = (void *)(nextLoadable->virt_addr + pg * PAGE_SIZE);
            ptm.MapMemory(virt_page, phys_page, true);
            memset(virt_page, 0, PAGE_SIZE);
            if(left_to_read > 0) {
                KERNEL_ASSERT(vf.seek(nextLoadable->location + pg * PAGE_SIZE) == VFS_OK);
                KERNEL_ASSERT(vf.read(virt_page, MIN(left_to_read, PAGE_SIZE)) == MIN(left_to_read, PAGE_SIZE));
            }
        }
    }

    *stack_top = USERLAND_STACK_TOP;
    process_set_pagetable((void *)current_pt);
    thread_entry->context->pml4 = (uint64_t)new_pt;
    thread_entry->context->virt_memory = (uint64_t *)new_pt;

    return 0;
}

void process_start(const char* executable, const char** argv) {
    tid_t my_thread = thread_get_current();
    uint64_t entry_point, stack_top;
    int ret = setup_new_process(my_thread, executable, argv, &entry_point, &stack_top);
    if(ret != 0) {
        return;
    }

    context_t userContext;
    memset(&userContext, 0, sizeof(userContext));

    context_set_ip(&userContext, entry_point);
    context_set_sp(&userContext, stack_top);
    pagetable_setcr3(thread_get_thread_entry(my_thread)->context->pml4);
    context_enter_userland(&userContext);
}

extern "C" void __idle_thread_wait_loop();

extern "C" void process_finish(int status) {
    GlobalRenderer->Printf("User process return with status %d", status);

    {
        WithInterrupts wi(false);

        scheduler_mark_finished();
    }

    __idle_thread_wait_loop();
    __builtin_unreachable();
}
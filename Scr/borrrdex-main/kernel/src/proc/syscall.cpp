#include "syscall.h"
#include "Panic.h"
#include "graphics/BasicRenderer.h"
#include "paging/PageTableManager.h"
#include "paging/PageFrameAllocator.h"
#include "thread.h"
#include <cstdint>
#include <vector>
static int do_write(int filehandle, const uint8_t* buffer, int length) {
    if(filehandle == STDIN_HANDLE) {
        return -1;
    }

    if(filehandle == STDOUT_HANDLE || filehandle == STDERR_HANDLE) {
        // TODO: Need length based string printf
        for(int i = 0; i < length; i++) {
            GlobalRenderer->PutChar(buffer[i]);
        }
    }

    return 0;
}

uintptr_t syscall_entry(uintptr_t syscall, uintptr_t a0, uintptr_t a1, uintptr_t a2) {
    switch(syscall) {
        case SYSCALL_WRITE:
            return do_write((int)a0, (const uint8_t *)a1, (int)a2);
        case SYSCALL_EXIT:
            return 0;
        // case SYSCALL_SBRK:
        // {
        //     auto* current = thread_get_current_thread_entry();
        //     void* virt_mem_start = (void *)(THREAD_VIRTUAL_HEAP_START + current->user_context->segments.size() * 0x1000);
        //     PageTableManager ptm((PageTable *)current->user_context->pml4);
        //     for(uintptr_t i = 0; i < a0; i++) {
        //         void* phys = PageFrameAllocator::SharedAllocator()->RequestPage();
        //         if(!phys) {
        //             return (uintptr_t)-1;
        //         }

        //         ptm.MapMemory(virt_mem_start, phys, true);
        //         virt_mem_start = (void *)((uint64_t)virt_mem_start + 0x1000);
        //         current->user_context->segments.push_back((uint64_t)phys);
        //     }
        // }
        default:
            Panic("Unhandled system call");
            break;
    }

    __builtin_unreachable();
}
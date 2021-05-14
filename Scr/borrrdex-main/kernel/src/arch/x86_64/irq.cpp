#include "irq.h"
#include "Panic.h"
#include "graphics/BasicRenderer.h"

#define HALT while(true) { asm volatile("hlt"); }
#define WRITEART(line) GlobalRenderer->Printf(line); GlobalRenderer->CursorPosition.y += 16; GlobalRenderer->CursorPosition.x -= 72

void print_common(regs_t* regs) {
    if(regs->error_code) {
        GlobalRenderer->Printf(" (code 0x%x [%llu])", regs->error_code, regs->error_code);
    }

    GlobalRenderer->Next();
    GlobalRenderer->Next();
    GlobalRenderer->Printf("Fault Location Information:\n");
    GlobalRenderer->Printf("RIP:    0x%016llx\t\tRSP: 0x%016llx\n", regs->rip, regs->rsp);
    GlobalRenderer->Printf("RFLAGS: 0x%016llx\t\tCS:  0x%016llx\t\tSS: 0x%016llx", regs->rflags, regs->cs, regs->ss);
    GlobalRenderer->Next();
    GlobalRenderer->Next();
    GlobalRenderer->Printf("Registers:\n\n");
    GlobalRenderer->Printf("RAX: 0x%016llx\t\tRBX: 0x%016llx\t\tRCX: 0x%016llx\t\tRDX: 0x%016llx\n", regs->rax, regs->rbx, regs->rcx, regs->rdx);
    GlobalRenderer->Printf("RSP: 0x%016llx\t\tRBP: 0x%016llx\t\tRSI: 0x%016llx\t\tRDI: 0x%016llx\n", regs->kernel_rsp, regs->rbp, regs->rsi, regs->rdi);
    GlobalRenderer->Printf("R8:  0x%016llx\t\tR9:  0x%016llx\t\tR10: 0x%016llx\t\tR11: 0x%016llx\n", regs->r8, regs->r9, regs->r10, regs->r11);
    GlobalRenderer->Printf("R12: 0x%016llx\t\tR13: 0x%016llx\t\tR14: 0x%016llx\t\tR15: 0x%016llx\n", regs->r12, regs->r13, regs->r14, regs->r15);
    GlobalRenderer->CursorPosition = {GlobalRenderer->Width() / 2 - 4 * 8, GlobalRenderer->Height() / 2 - 2 * 16};
    WRITEART(" _______");
    WRITEART("|.-----.|");
    WRITEART("||x . x||");
    WRITEART("||_.-._||");
    WRITEART("`--)-(--`");
}

extern "C" void interrupt_handle(regs_t* regs) {
    switch(regs->irq) {
        case 0:
            Panic("Hardware exception #DE");
            break;
        case 1:
            Panic("Hardware exception #DB");
            break;
        case 2:
            Panic("NMI Unhandled");
            break;
        case 3:
            Panic("Hardware exception #BP");
            break;
        case 4:
            Panic("Hardware exception #OF");
            break;
        case 5:
            Panic("Hardware exception #BR");
            break;
        case 6:
            Panic("Hardware exception #UD");
            break;
        case 7:
            Panic("Hardware exception #NM");
            break;
        case 8:
            Panic("Hardware exception #DF");
            break;
        case 9:
            Panic("Ancient FPU based GPD, why?");
            break;
        case 10:
            Panic("Hardware exception #TS");
            break;
        case 11:
            Panic("Hardware exception #NP");
            break;
        case 12:
            Panic("Hardware exception #SS");
            break;
        case 13:
            Panic("Hardware exception #GP");
            break;
        case 14:
            Panic("Hardware exception #PF");
            uint64_t page;
            asm volatile("movq %%cr2, %%rdx" : "=d"(page));

            GlobalRenderer->Printf(" requesting address 0x%llx", page);
            break;
        case 15:
            Panic("Ancient FPU based error, why?");
            break;
        case 16:
            Panic("Hardware exception #MF");
            break;
        case 17:
            Panic("Hardware exception #AC");
            break;
        case 18:
            Panic("Hardware exception #MC");
            break;
        case 19:
            Panic("Hardware exception #XM");
            break;
    }

    print_common(regs);
    HALT
}

#undef HALT
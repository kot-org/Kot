#include "panic.h"

void KernelPanic(const char* Message,IntreruptFrame* frame) {
    CPURegisters registers = DumpRegisters();
    GlobalCOM1->Write(Message);
    slowmemset(GlobalDisplay->secondFrameBuffer->BaseAddr,0xFF,GlobalDisplay->secondFrameBuffer->BufferSize);
    GlobalDisplay->setCursorPos(0,0);
    GlobalDisplay->setColour(0xffffff);
    GlobalDisplay->highlightputs(0,"LLOS\n");
    GlobalDisplay->highlightputs(0,"A kernel panic occured!\n\n");
    GlobalDisplay->highlightputs(0,"Reason: ");
    GlobalDisplay->highlightputs(0,Message);

    GlobalDisplay->highlightputs(0,"\n\nCPU Registers: \n");
    GlobalDisplay->highlightputs(0,"RAX: ");
    GlobalDisplay->highlightputs(0,inttohstr(registers.rax));
    GlobalDisplay->highlightputs(0,"\n");
    GlobalDisplay->highlightputs(0,"RBX: ");
    GlobalDisplay->highlightputs(0,inttohstr(registers.rbx));
    GlobalDisplay->highlightputs(0,"\n");
    GlobalDisplay->highlightputs(0,"RCX: ");
    GlobalDisplay->highlightputs(0,inttohstr(registers.rcx));
    GlobalDisplay->highlightputs(0,"\n");
    GlobalDisplay->highlightputs(0,"RDX: ");
    GlobalDisplay->highlightputs(0,inttohstr(registers.rdx));
    GlobalDisplay->highlightputs(0,"\n");
    GlobalDisplay->highlightputs(0,"RSI: ");
    GlobalDisplay->highlightputs(0,inttohstr(registers.rsi));
    GlobalDisplay->highlightputs(0,"\n");
    GlobalDisplay->highlightputs(0,"RSP: ");
    GlobalDisplay->highlightputs(0,inttohstr(registers.rsp));
    GlobalDisplay->highlightputs(0,"\n");
    GlobalDisplay->highlightputs(0,"RBP: ");
    GlobalDisplay->highlightputs(0,inttohstr(registers.rbp));
    GlobalDisplay->highlightputs(0,"\n");
    GlobalDisplay->highlightputs(0,"R8:  ");
    GlobalDisplay->highlightputs(0,inttohstr(registers.r8));
    GlobalDisplay->highlightputs(0,"\n");
    GlobalDisplay->highlightputs(0,"R9:  ");
    GlobalDisplay->highlightputs(0,inttohstr(registers.r9));
    GlobalDisplay->highlightputs(0,"\n");
    GlobalDisplay->highlightputs(0,"R10: ");
    GlobalDisplay->highlightputs(0,inttohstr(registers.r10));
    GlobalDisplay->highlightputs(0,"\n");
    GlobalDisplay->highlightputs(0,"R11: ");
    GlobalDisplay->highlightputs(0,inttohstr(registers.r11));
    GlobalDisplay->highlightputs(0,"\n");
    GlobalDisplay->highlightputs(0,"R12: ");
    GlobalDisplay->highlightputs(0,inttohstr(registers.r12));
    GlobalDisplay->highlightputs(0,"\n");
    GlobalDisplay->highlightputs(0,"R13: ");
    GlobalDisplay->highlightputs(0,inttohstr(registers.r13));
    GlobalDisplay->highlightputs(0,"\n");
    GlobalDisplay->highlightputs(0,"R14: ");
    GlobalDisplay->highlightputs(0,inttohstr(registers.r14));
    GlobalDisplay->highlightputs(0,"\n");
    GlobalDisplay->highlightputs(0,"R15: ");
    GlobalDisplay->highlightputs(0,inttohstr(registers.r15));
    GlobalDisplay->highlightputs(0,"\n\n");

    GlobalDisplay->highlightputs(0,"Intrerupt Stack Frame: \n");
    GlobalDisplay->highlightputs(0,"CS: ");
    GlobalDisplay->highlightputs(0,inttohstr(frame->cs));
    GlobalDisplay->highlightputs(0,"\n");
    GlobalDisplay->highlightputs(0,"Flags: ");
    GlobalDisplay->highlightputs(0,inttohstr(frame->flags));
    GlobalDisplay->highlightputs(0,"\n");
    GlobalDisplay->highlightputs(0,"IP: ");
    GlobalDisplay->highlightputs(0,inttohstr(frame->ip));
    GlobalDisplay->highlightputs(0,"\n");
    GlobalDisplay->highlightputs(0,"SP: ");
    GlobalDisplay->highlightputs(0,inttohstr(frame->sp));
    GlobalDisplay->highlightputs(0,"\n");
    GlobalDisplay->highlightputs(0,"SS: ");
    GlobalDisplay->highlightputs(0,inttohstr(frame->ss));
    GlobalDisplay->highlightputs(0,"\n");
    GlobalDisplay->update();
    while(1) {
        asm volatile ("hlt");
    }
}
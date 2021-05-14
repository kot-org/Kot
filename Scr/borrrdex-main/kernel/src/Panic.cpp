#include "Panic.h"
#include "graphics/BasicRenderer.h"

void Panic(const char* panicMessage) {
    GlobalRenderer->SetBackground(0x00ff0000);
    GlobalRenderer->Clear();
    GlobalRenderer->CursorPosition = {0, 0};
    GlobalRenderer->SetColor(0);
    GlobalRenderer->Printf("Kernel Panic\n\n");

    GlobalRenderer->Printf(panicMessage);
}

void AssertionPanic(const char* file, int line, const char* message) {
    GlobalRenderer->SetBackground(0x00ff0000);
    GlobalRenderer->Clear();
    GlobalRenderer->CursorPosition = {0, 0};
    GlobalRenderer->SetColor(0);
    GlobalRenderer->Printf("Kernel Panic\n\n");

    GlobalRenderer->Printf("Assertion Failed: %s (%s:%d)", message, file, line);
    while(true) {
        asm volatile("hlt");
    }
}
#include "panic.h"
#include "../graphics.h"

void Panic(char* panicMessage){
    globalGraphics->ClearColor = 0x0002818F;
    globalGraphics->Clear();
    globalGraphics->CursorPosition = {0, 0};
    globalGraphics->Color = 0xffffffff;
    globalGraphics->Print("Kernel panic");
    globalGraphics->Next();
    globalGraphics->Next();
    globalGraphics->Print(panicMessage);
    globalGraphics->Update();
}
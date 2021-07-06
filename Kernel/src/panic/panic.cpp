#include "panic.h"
#include "../graphics.h"

void Panic(char* panicMessage){
    globalGraphics->Next();
    globalGraphics->Color = 0xffffff00;
    globalGraphics->Print(panicMessage);
    globalGraphics->Color = 0xffffffff;
    globalGraphics->Next();
    globalGraphics->Update();
}
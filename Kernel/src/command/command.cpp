#include "command.h"

char commandLine;

void WriteCommand(char newChar){
    commandLine = newChar;
}
void SendCommand(){
    switch(commandLine){
        case 'r':
            globalGraphics->Next();

            globalGraphics->Print("Total RAM: ");
            globalGraphics->Print(to_string(globalAllocator.GetTotalRAM() / 1024 ));
            globalGraphics->Print(" KB ");
            globalGraphics->Next();

            globalGraphics->Print("Free RAM: ");
            globalGraphics->Print(to_string(globalAllocator.GetFreeRAM() / 1024));
            globalGraphics->Print(" KB ");
            globalGraphics->Next();

            globalGraphics->Print("Used RAM: ");
            globalGraphics->Print(to_string(globalAllocator.GetUsedRAM() / 1024));
            globalGraphics->Print(" KB ");
            globalGraphics->Next();

            globalGraphics->Print("Reserved RAM: ");
            globalGraphics->Print(to_string(globalAllocator.GetReservedRAM() / 1024));
            globalGraphics->Print(" KB ");
    }    
    globalGraphics->Next();
    globalGraphics->Print("Kot > ");    
}
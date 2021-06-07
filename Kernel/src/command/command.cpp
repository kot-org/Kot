#include "command.h"

char commandLine;

void WriteCommand(char newChar){
    commandLine = newChar;
}
void SendCommand(){
    globalGraphics->Next();
    globalGraphics->Print("Kot > ");    
}
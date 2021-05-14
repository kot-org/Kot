#include "log.h"

void Logging::info(const char* text) {
    GlobalDisplay->puts("[");
    GlobalDisplay->setColour(LIGHTBLUE);
    GlobalDisplay->puts("INFO");
    GlobalDisplay->setColour(WHITE);
    GlobalDisplay->puts("] ");
    GlobalDisplay->puts(text);
    GlobalDisplay->cursorNewLine();
    GlobalDisplay->update();
}

void Logging::warn(const char* text) {
    GlobalDisplay->puts("[");
    GlobalDisplay->setColour(YELLOW);
    GlobalDisplay->puts("WARN");
    GlobalDisplay->setColour(WHITE);
    GlobalDisplay->puts("] ");
    GlobalDisplay->puts(text);
    GlobalDisplay->cursorNewLine();
    GlobalDisplay->update();
}

void Logging::error(const char* text) {
    GlobalDisplay->puts("[");
    GlobalDisplay->setColour(LIGHTRED);
    GlobalDisplay->puts("ERROR");
    GlobalDisplay->setColour(WHITE);
    GlobalDisplay->puts("] ");
    GlobalDisplay->puts(text);
    GlobalDisplay->cursorNewLine();
    GlobalDisplay->update();  
}
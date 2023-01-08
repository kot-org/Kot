#include <main/main.h>

#include <kot++/java/se8/jvm.h>
using namespace SE8;

#include <kot++/printf.h>
using namespace std;

kfont_t* Font;

extern "C" int main() {    
    // Load font
    file_t* FontFile = fopen("default-font.sfn", "r");
    fseek(FontFile, 0, SEEK_END);
    size64_t Size = ftell(FontFile);
    uintptr_t Buffer = malloc(Size);
    fseek(FontFile, 0, SEEK_SET);
    fread(Buffer, Size, 1, FontFile);
    Font = (kfont_t*)LoadFont(Buffer);

    NewShell();

    return KSUCCESS;
}
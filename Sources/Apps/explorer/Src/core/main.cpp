#include <core/main.h>

extern "C" int main() {
    UiWindow::Window* CalculatorWindow = new UiWindow::Window("File explorer", "explorer.tga", 600, 600, 600, 10);
    
    return KSUCCESS;
}
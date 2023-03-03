#include <core/main.h>

#include <display/display.h>

extern "C" int main() {
    UiWindow::Window* CalculatorWindow = new UiWindow::Window("Calculator", "calculator.tga", 400, 600, 10, 10);
    CreateDisplay(CalculatorWindow->Cpnt);

    return KSUCCESS;
}
#include <core/main.h>

#include <display/display.h>

#include <kot++/printf.h>

double Number = 0;

static bool InOperation = false;
static char CurrentOperator = NULL;
static bool NumberIsDecimal = false;
static double DecimalMultiplier = 0.1;

static bool IsDigit(char character) {
    if(character < '0' || character > '9')
            return false;
    return true;
}

static bool IsOperator(char Operator) {
    if(Operator == ADD_OP || Operator == SUB_OP || Operator == MULT_OP || Operator == DIV_OP || Operator == SQUARE_OP)
        return true;
    return false;
}

static uint8_t OrderOfOperations(char Operator) {
    if(Operator == SQUARE_OP) return 3;
    else if(Operator == MULT_OP || Operator == DIV_OP) return 2;
    else if(Operator == ADD_OP || Operator == SUB_OP) return 1;
    else return -1;
}

void AddDigit(uint8_t Digit) {
    std::printf("%d", Digit);
    if(NumberIsDecimal) {
        Number = Number + DecimalMultiplier * Digit;
        DecimalMultiplier *= 0.1;
    } else {
        Number = Number * 10 + Digit;
    }

    // todo nombre max

    DisplayNumber(Number);
}

void DoOperationOrAction(char Operator) {
    
    /* OPERATION */
    if(IsOperator(Operator)) {
        ClearCalculator();
        CurrentOperator = Operator;
        DisplayNumber(0);
    } else {
        /* ACTION */
        if(Operator == CLEARALL_ACTION) {

        } else if(Operator == CLEAR_ACTION) {

        } else if(Operator == BACKSPACE_ACTION) {

        } else if(Operator == DECIMAL_ACTION) {
            if(InOperation)
                AddDigit(0);
            NumberIsDecimal = true;
        } else if(Operator == EQUAL_ACTION) {

        } else {
            // display error
        }
    }
    
        
}

void CalculateExpression(char* Expression) {
    
}

void ClearCalculator() {
    CurrentOperator = NULL;
    Number = 0;
    NumberIsDecimal = false;
    DecimalMultiplier = 0.1;
}

int main() {
    UiWindow::Window* CalculatorWindow = new UiWindow::Window("Calculator", "d0:/calculator.tga", 400, 600, 10, 10);

    CreateDisplay(CalculatorWindow->Cpnt);

    return KSUCCESS;
}
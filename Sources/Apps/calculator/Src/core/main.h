#pragma once

#include <stdio.h>
#include <assert.h>

#include <kot/sys.h>
#include <kot/cstring.h>

#include <kot++/new.h>

#include <kot-ui++/window.h>

#define CLEARALL_ACTION     'A'
#define CLEAR_ACTION        'B'
#define BACKSPACE_ACTION    'C'
#define DECIMAL_ACTION      '.'
#define EQUAL_ACTION        '='

#define PERCENT_OP          '%'
#define RESIPROCAL_OP       'R'
#define SQUARE_OP           '^'
#define SQRT_OP             'S'
#define DIV_OP              '/'
#define MULT_OP             '*'
#define SUB_OP              '-'
#define ADD_OP              '+'
#define INVERTSIGN_OP       'I'

/* enum Operators {
    NONE    = 0,

    SQUARE  = 1,
    SQRT    = 2,
    MULT    = 3,
    DIV     = 4,
    ADD     = 5,
    SUB     = 6,

    PERCENT     = 7,
    INVERTSIGN  = 8,
    RESIPROCAL  = 9
};

enum Actions {
    CLEARALL    = 10,
    CLEAR       = 11,
    BACKSPACE   = 12,
    DECIMAL     = 13,
    EQUAL       = 14
};

enum Digits {
    DIGIT0  = 15,
    DIGIT1  = 16,
    DIGIT2  = 17,
    DIGIT3  = 18,
    DIGIT4  = 19,
    DIGIT5  = 20,
    DIGIT6  = 21,
    DIGIT7  = 22,
    DIGIT8  = 23,
    DIGIT9  = 24
}; */

void AddDigit(uint8_t Digit);
void DoOperationOrAction(char Operator);
void CalculateExpression(char* Expression);

void ClearCalculator();
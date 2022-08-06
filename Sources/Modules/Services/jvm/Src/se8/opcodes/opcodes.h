#pragma once

#include <kot/types.h>
class SE8;

void iload_fn(SE8* jvm, uint8_t* operands, bool isWide);
void istore_fn(SE8* jvm, uint8_t* operands, bool isWide);

void iadd_fn(SE8* jvm, uint8_t* operands, bool isWide);
void isub_fn(SE8* jvm, uint8_t* operands, bool isWide);
void imul_fn(SE8* jvm, uint8_t* operands, bool isWide);
void idiv_fn(SE8* jvm, uint8_t* operands, bool isWide);

enum SE8OpCodes {
    /* int type (32bits) */
    // memory
    iload = 0x15,
    istore = 0x36,
    // math
    iadd = 0x60,
    isub = 0x64,
    imul = 0x68,
    idiv = 0x6c,
    // if<cond>
    ifeq = 0x99, // value == 0
    ifne = 0x9a, // value != 0
    iflt = 0x9b, // value < 0
    ifge = 0x9c, // value <= 0
    ifgt = 0x9d, // value > 0
    ifle = 0x9e, // value >= 0
};
#pragma once

#include <kot/utils/vector.h>

#include <jvm8/stack.h>
#include <jvm8/localVariable.h>
#include <jvm8/methodArea.h>

struct JVM8OpCode;

enum JVM8OpCodes {
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

class JVM8 {
public:
    void register_opcode(JVM8OpCodes opcode, void (*fn_)(JVM8* jvm, uint8_t* operands, bool isWide), size_t byteLength);
    uint64_t pc = 0;
    JVM8Stack* stack;
    JVM8LocalVariable* lv;
    JVM8MethodArea* methodArea;
    JVM8OpCode** opCodesTable;
    JVM8(size_t stackCapacity);
    void initialize();
};

struct JVM8OpCode {
    size_t byteLength;
    void (*fn)(JVM8* jvm, uint8_t* operands, bool isWide);
};
#include <jvm8/jvm.h>

/**
 * @brief Construct a new Java Virtual Machine implemented with SE8 specs. 
 * The stack capacity is by default set to 512kb of memory (-Xss args)
 */
JVM8::JVM8(size_t stackCapacity) {
    this->lv = new JVM8LocalVariable();
    this->stack = new JVM8Stack(stackCapacity);
    this->methodArea = new JVM8MethodArea();
    this->opCodesTable = (JVM8OpCode**) malloc(sizeof(JVM8OpCode*)*512);
}

void iload_fn(JVM8* jvm, uint8_t* operands, bool isWide) {
    jvm->stack->pushInt(*jvm->lv->getInt(operands[0]));
}

void istore_fn(JVM8* jvm, uint8_t* operands, bool isWide) {
    jvm->lv->setInt(operands[0], jvm->stack->popInt());
}

void iadd_fn(JVM8* jvm, uint8_t* operands, bool isWide) {
    int32_t value2 = jvm->stack->popInt();
    int32_t value1 = jvm->stack->popInt();
    jvm->stack->pushInt(value1 + value2);
}

void isub_fn(JVM8* jvm, uint8_t* operands, bool isWide) {
    int32_t value2 = jvm->stack->popInt();
    int32_t value1 = jvm->stack->popInt();
    jvm->stack->pushInt(value1 - value2);
}

void imul_fn(JVM8* jvm, uint8_t* operands, bool isWide) {
    int32_t value2 = jvm->stack->popInt();
    int32_t value1 = jvm->stack->popInt();
    jvm->stack->pushInt(value1 * value2);
}

void idiv_fn(JVM8* jvm, uint8_t* operands, bool isWide) {
    int32_t value2 = jvm->stack->popInt();
    int32_t value1 = jvm->stack->popInt();
    jvm->stack->pushInt(value1 / value2);
}

void ifeq_fn(JVM8* jvm, uint8_t* operands, bool isWide) {
    int32_t value = jvm->stack->popInt();
    if (value == 0) {
        int16_t offset = (operands[0] << 8) | operands[1];
        jvm->pc += offset;
    }
}

void ifne_fn(JVM8* jvm, uint8_t* operands, bool isWide) {
    int32_t value = jvm->stack->popInt();
    if (value != 0) {
        int16_t offset = (operands[0] << 8) | operands[1];
        jvm->pc += offset;
    }
}

void iflt_fn(JVM8* jvm, uint8_t* operands, bool isWide) {
    int32_t value = jvm->stack->popInt();
    if (value < 0) {
        int16_t offset = (operands[0] << 8) | operands[1];
        jvm->pc += offset;
    }
}

void ifge_fn(JVM8* jvm, uint8_t* operands, bool isWide) {
    int32_t value = jvm->stack->popInt();
    if (value <= 0) {
        int16_t offset = (operands[0] << 8) | operands[1];
        jvm->pc += offset;
    }
}

void ifgt_fn(JVM8* jvm, uint8_t* operands, bool isWide) {
    int32_t value = jvm->stack->popInt();
    if (value > 0) {
        int16_t offset = (operands[0] << 8) | operands[1];
        jvm->pc += offset;
    }
}

void ifle_fn(JVM8* jvm, uint8_t* operands, bool isWide) {
    int32_t value = jvm->stack->popInt();
    if (value >= 0) {
        int16_t offset = (operands[0] << 8) | operands[1];
        jvm->pc += offset;
    }
}

void JVM8::register_opcode(JVM8OpCodes opcode, void (*fn)(JVM8* jvm, uint8_t* operands, bool isWide), size_t byteLength) {
    JVM8OpCode* opc = (JVM8OpCode*) malloc(sizeof(JVM8OpCode));
    opc->byteLength = byteLength;
    opc->fn = fn;
    this->opCodesTable[opcode] = opc;
}

void JVM8::initialize() {
    // mem
    register_opcode(JVM8OpCodes::iload, &iload_fn, 1);
    register_opcode(JVM8OpCodes::istore, &istore_fn, 1);
    // math
    register_opcode(JVM8OpCodes::iadd, &iadd_fn, 0);
    register_opcode(JVM8OpCodes::isub, &isub_fn, 0);
    register_opcode(JVM8OpCodes::imul, &imul_fn, 0);
    register_opcode(JVM8OpCodes::idiv, &idiv_fn, 0);
    // if<cond>
    register_opcode(JVM8OpCodes::ifeq, &ifeq_fn, 2);
    register_opcode(JVM8OpCodes::ifne, &ifne_fn, 2);
    register_opcode(JVM8OpCodes::iflt, &iflt_fn, 2);
    register_opcode(JVM8OpCodes::ifge, &ifge_fn, 2);
    register_opcode(JVM8OpCodes::ifgt, &ifgt_fn, 2);
    register_opcode(JVM8OpCodes::ifle, &ifle_fn, 2);
}

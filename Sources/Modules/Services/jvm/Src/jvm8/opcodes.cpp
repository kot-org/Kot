#pragma once

#include <jvm8/opcodes.h>

JVM8OpCodesTable::JVM8OpCodesTable() {
    this->table = vector_create(sizeof(JVM8Instruction));
}

void JVM8OpCodesTable::set(uint64_t opcode, JVM8Instruction* instr) {
    if (table->length == 0) {
        vector_expand(table, opcode);
    } else if (table->length <= opcode) {
        vector_expand(table, opcode-(table->length-1));
    }
    vector_set(table, opcode, instr);
}
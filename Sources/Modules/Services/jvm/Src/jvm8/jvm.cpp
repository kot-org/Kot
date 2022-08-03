#include <jvm8/jvm.h>

/**
 * @brief Construct a new Java Virtual Machine implemented with SE8 specs. 
 * The stack capacity is by default set to 512kb of memory (-Xss args)
 */
JVM8::JVM8(size_t stackCapacity) {
    this->heap = new JVM8Heap();
    this->stack = new JVM8Stack(stackCapacity);
    this->methodArea = new JVM8MethodArea();
    this->opCodesTable = new JVM8OpCodesTable();
}

void JVM8::initialize() {
    JVM8Instruction* iload = (JVM8Instruction*) malloc(sizeof(JVM8Instruction));
    iload->byteLength = 2;
    this->opCodesTable->set(JVM8OpCodes::iload, iload);
}
#include "jvm.h"

namespace SE8 {

    JVM::JVM() {
        this->classes = new Classes();
        this->threads = new Threads();
    }

    void JVM::loadClassBytes(uintptr_t bytes) {
        classes->loadClassBytes(bytes);
    }

    void JVM::run() {
        this->threads->create("main");
        // todo
    }

    size_t JVM::getStackSize() {
        return this->stack_size;
    }

    void JVM::setStackSize(size_t size) {
        this->stack_size = size;
    }

}
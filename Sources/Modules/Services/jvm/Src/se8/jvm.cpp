#include "jvm.h"

namespace SE8 {

    JavaVM::JavaVM() {
        this->classes = new Classes();
        this->threads = new Threads();
        if (isOpCodeTableNotInit()) {
            initOpCodeTable();
        }
    }

    Classes* JavaVM::getClasses() {
        return this->classes;
    }

    Threads* JavaVM::getThreads() {
        return this->threads;
    }

    void JavaVM::run(Value* args, uint32_t args_length) {
        this->classes->clinit(this);
        this->classes->getClass(this->entryPoint)->getEntryPoint(this)->run(args, args_length);
    }

    void JavaVM::setEntryPoint(char* entryPoint) {
        this->entryPoint = entryPoint;
    }

}
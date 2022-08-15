#include "jvm.h"

namespace SE8 {

    JVM::JVM() {
        this->classes = new Classes();
        this->threads = new Threads();
    }

    Classes* JVM::getClasses() {
        return this->classes;
    }

    Threads* JVM::getThreads() {
        return this->threads;
    }

    void JVM::run() {
        this->classes->clinit();
    }

}
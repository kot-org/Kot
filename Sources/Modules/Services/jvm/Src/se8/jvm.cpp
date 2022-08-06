#include "jvm.h"

JVM8::JVM8() {
    this->classes = new SE8Class();
}

void JVM8::loadClassFile(uintptr_t bytes) {
    classes->loadClassFile(bytes);
}

void JVM8::loadJarFile() {
    classes->loadJarFile();
}
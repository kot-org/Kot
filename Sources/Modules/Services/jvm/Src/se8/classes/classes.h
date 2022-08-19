#pragma once

#include "class.h"

namespace SE8 {

    class ClassArea;
    class JavaVM;

    class Classes {
    private:
        JavaVM* jvm;
        vector_t* areas;
    public:
        Classes(JavaVM* jvm);
        ClassArea* getClass(char* name);
        void loadClassBytes(uintptr_t bytes);
        void clinit();
    };

}
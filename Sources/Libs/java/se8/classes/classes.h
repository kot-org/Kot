#pragma once

#include "class.h"

#ifndef __SE8_CLASSES_H__
#define __SE8_CLASSES_H__

namespace SE8 {

    class ClassArea;
    class JavaVM;

    class Classes {
    private:
        JavaVM* jvm;
        kot_vector_t* areas;
    public:
        Classes(JavaVM* jvm);
        ClassArea* getClass(char* name);
        void loadClassBytes(uintptr_t bytes);
        void clinit();
    };

}

#endif
#pragma once

#include "class.h"

#include <kot/utils/map.h>

namespace SE8 {

    class Class;

    class Classes {
    private:
        vector_t* areas;
    public:
        Classes();
        Class* getClass(char* name);
        Method* getMethod(char* className, char* methodName, char* descriptor);
        Method* getStaticMethod(char* className, char* methodName, char* descriptor);
        void loadClassBytes(uintptr_t bytes);
        void clinit();
    };

}
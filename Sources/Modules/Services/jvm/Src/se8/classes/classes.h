#pragma once

#include "class.h"

namespace SE8 {

    class Class;
    class JavaVM;

    class Classes {
    private:
        vector_t* areas;
    public:
        Classes();
        Class* getClass(char* name);
        Method* getMethod(char* className, char* methodName, char* descriptor);
        Method* getStaticMethod(char* className, char* methodName, char* descriptor);
        void setStaticField(char* className, char* fieldName, Value* value);
        Value* getStaticField(char* className, char* fieldName);
        void loadClassBytes(uintptr_t bytes);
        void clinit(JavaVM* jvm);
    };

}
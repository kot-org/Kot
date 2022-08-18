#pragma once

#include "../types.h"
#include "../frame/frame.h"

namespace SE8 {

    class Frame;
    class JavaVM;

    class Class {
    private:
        uint32_t magic;
        uint16_t minorVersion;
        uint16_t majorVersion;
        uint16_t constant_pool_count;
        uintptr_t* constant_pool;
        uint16_t access_flags;
        uint16_t this_class;
        uint16_t super_class;
        uint16_t interfaces_count;
        uint16_t* interfaces;
        uint16_t fields_count;
        FieldInfo** fields;
        uint16_t methods_count;
        Method** methods;
        uint16_t attributes_count;
        Attribute** attributes;
        Attribute** parseAttributes(uint16_t attributes_count, Reader* reader);
        vector_t* runtime_static_fields;
    public:
        Class(uintptr_t bytes);
        char* getSourceFileName();
        char* getClassName();
        char* getSuperClassName();
        uint16_t getAccessFlags();
        Method* getStaticMethod(char* name, char* descriptor);
        Method* getMethod(char* name, char* descriptor);
        Frame* getEntryPoint(JavaVM* jvm);
        Method** getMethods();
        uintptr_t* getConstantPool();
        void setStaticField(char* fieldName, Value* value);
        Value* getStaticField(char* fieldName);
    };

}
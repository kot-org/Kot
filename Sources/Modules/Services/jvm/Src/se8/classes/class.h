#pragma once

#include "../types.h"

namespace SE8 {

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
        MethodInfo** methods;

        uint16_t attributes_count;
        Attribute** attributes;

        Attribute** parseAttributes(uint16_t attributes_count, Reader* reader);
        
    public:

        Class(uintptr_t bytes);

        char* getClassName();
        char* getSuperClassName();

        uintptr_t* getConstantPool();

    };

}

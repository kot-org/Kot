#pragma once

#include "../types.h"
#include "../frame/frame.h"
#include "area.h"

namespace SE8 {

    class Frame;
    class JavaVM;

    class ClassParser {
    private:
        Attribute** parseAttributes(uint16_t attributes_count, Reader* reader);
    public:
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
        ClassParser(uintptr_t bytes);
        uint8_t getFieldSize(char* fieldName);
        uint8_t getStaticFieldSize(char* fieldName);
    };

}
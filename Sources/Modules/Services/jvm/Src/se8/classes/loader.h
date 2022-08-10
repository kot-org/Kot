#pragma once

#include <utils/reader.h>
#include <kot/heap.h>

namespace SE8 {

    enum ConstantPoolTags {
        CONSTANT_Class = 7,
        CONSTANT_Fieldref = 9,
        CONSTANT_Methodref = 10,
        CONSTANT_InterfaceMethodref = 11,
        CONSTANT_String = 8,
        CONSTANT_Integer = 3,
        CONSTANT_Float = 4,
        CONSTANT_Long = 5,
        CONSTANT_Double = 6,
        CONSTANT_NameAndType = 12,
        CONSTANT_Utf8 = 1,
        CONSTANT_MethodHandle = 15,
        CONSTANT_MethodType	= 16,
        CONSTANT_InvokeDynamic = 18,
    };

    enum AccessFlags {
        ACC_PUBLIC = 0x0001,
        ACC_PRIVATE = 0x002,
        ACC_PROTECTED = 0x0004,
        ACC_STATIC = 0x0008,
        ACC_FINAL = 0x0010,
        ACC_SUPER = 0x0020,
        ACC_VOLATILE = 0x0040,
        ACC_TRANSIENT = 0x0080,
        ACC_INTERFACE = 0x0200,
        ACC_ABSTRACT = 0x0400,
        ACC_SYNTHETIC = 0x1000,
        ACC_ANNOTATION = 0x2000,
        ACC_ENUM = 0x4000,
        ACC_SYNCHRONIZED = 0x0020,
        ACC_BRIDGE = 0x0040,
        ACC_VARARGS = 0x0080,
        ACC_NATIVE = 0x0100,
        ACC_STRICT = 0x0800,
    };

    struct ConstantPoolEntry {
        uint8_t tag;
    };

    struct Constant_ClassInfo {
        uint8_t tag;
        uint16_t name_index;
    };

    struct Constant_RefInfo {
        uint8_t tag;
        uint16_t class_index;
        uint16_t name_and_type_index;
    };

    struct Constant_Utf8 {
        uint8_t tag;
        uint16_t length;
        uint8_t* bytes;
    };

    struct Constant_NameAndType {
        uint8_t tag;
        uint16_t name_index;
        uint16_t descriptor_index;
    };

    struct Constant_String {
        uint8_t tag;
        uint16_t string_index;
    };

    struct Constant_Integer_Float {
        uint8_t tag;
        uint32_t bytes;
    };

    struct Constant_Double_Long {
        uint8_t tag;
        uint32_t high_bytes;
        uint32_t low_bytes;
    };

    struct Constant_MethodHandle {
        uint8_t tag;
        uint8_t reference_kind;
        uint16_t reference_index;
    };

    struct Constant_Type {
        uint8_t tag;
        uint16_t descriptor_index;
    };

    struct Constant_InvokeDynamic {
        uint8_t tag;
        uint16_t bootstrap_method_attr_index;
        uint16_t name_and_type_index;
    };

    enum AttributeInfo_Type {
        Attribute_ConstantValue,
    };

    struct AttributeInfo {
        uint16_t attribute_name_index;
        AttributeInfo_Type type;
    };

    struct AttributeInfo_ConstantValue {
        uint16_t attribute_name_index;
        AttributeInfo_Type type;
        uint16_t constantvalue_index;
    };

    struct FieldInfo {
        uint16_t access_flags;
        uint16_t name_index;
        uint16_t descriptor_index;
        uint16_t attributes_count;
        AttributeInfo** attributes;
    };

    struct MethodInfo {
        uint16_t access_flags;
        uint16_t name_index;
        uint16_t descriptor_index;
        uint16_t attributes_count;
        AttributeInfo** attributes;
    };

    class ClassLoader {
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
        AttributeInfo** attributes;

        AttributeInfo_Type getAttributeType(uint16_t attribute_name_index);

    public:

        ClassLoader(uintptr_t bytes);

        uint8_t* getClassName();
        uint8_t* getSuperClassName();

        uintptr_t* getConstantPool();

    };

}

#pragma once 

#include "../types.h"

namespace SE8 {

    class Frame;
    class JavaVM;
    class ClassParser;

    struct FieldDescriptor {
        uint8_t size;
        uint32_t index;
    };

    struct ByteCodeMethod {
        uint16_t max_stack;
        uint16_t max_locals;
        uint32_t code_length;
        uintptr_t code;
    };

    struct NativeMethod {
        uint32_t (*fn)(JavaVM* jvm, uint32_t object, uint32_t* args, uint16_t args_length);
    };

    struct MethodInterface {
        uint8_t type; // 0: bytecode ; 1: native
        uint16_t args_length;
        uintptr_t method;
    };

    class ClassArea {
    private:

        ClassArea* super_class;

        char* name;
        uintptr_t* constant_pool;

        uintptr_t static_object;

        kot_vector_t* static_fields_map = map_create();
        uint32_t static_fields_size = 0;

        kot_vector_t* static_methods_map = map_create();

    public:

        kot_vector_t* fields_map = map_create();
        uint32_t fields_size = 0;

        kot_vector_t* methods_map = map_create();

        ClassArea(ClassArea* super_class, char* name);   
        ClassArea(ClassParser* cl);

        uint16_t getMethodArgsLength(char* name, char* signature);
        uint16_t getStaticMethodArgsLength(char* name, char* signature);

        bool isMethodStatic(char* name, char* signature);

        char* getName();
        uintptr_t* getConstantPool();

        uintptr_t newObject();
        uintptr_t newStaticObject();

        void registerMethod(char* name, char* signature, MethodInterface* interface);
        void registerStaticMethod(char* name, char* signature, MethodInterface* interface);
        
        uint32_t runMethod(JavaVM* jvm, uint32_t object, char* name, char* signature, uint32_t* args, uint16_t args_length);
        uint32_t runStaticMethod(JavaVM* jvm, char* name, char* signature, uint32_t* args, uint16_t args_length);

        void registerField(char* name, uint8_t size);

        uint8_t getFieldSize(char* name);

        uint8_t getField8(uintptr_t object, char* name);
        uint16_t getField16(uintptr_t object, char* name);
        uint32_t getField32(uintptr_t object, char* name);
        uint64_t getField64(uintptr_t object, char* name);
        
        void setField8(uintptr_t object, char* name, uint8_t item);
        void setField16(uintptr_t object, char* name, uint16_t item);
        void setField32(uintptr_t object, char* name, uint32_t item);
        void setField64(uintptr_t object, char* name, uint64_t item);

        void registerStaticField(char* name, uint8_t size);

        uint8_t getStaticFieldSize(char* name);

        uint8_t getStaticField8(char* name);
        uint16_t getStaticField16(char* name);
        uint32_t getStaticField32(char* name);
        uint64_t getStaticField64(char* name);
        
        void setStaticField8(char* name, uint8_t item);
        void setStaticField16(char* name, uint16_t item);
        void setStaticField32(char* name, uint32_t item);
        void setStaticField64(char* name, uint64_t item);

        void clinit(JavaVM* jvm);

    };

}
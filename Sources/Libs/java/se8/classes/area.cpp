#include "area.h"

namespace SE8 {

    ClassArea::ClassArea(ClassArea* super_class, char* name) {
        this->super_class = super_class;
        this->name = name;
        this->constant_pool = NULL;
        if (super_class != NULL) {
            // todo
        }
    }

    ClassArea::ClassArea(ClassParser* cl) {

        // auto register

        this->super_class = NULL;

        this->name = (char*) ((Constant_Utf8*) cl->constant_pool[((Constant_ClassInfo*) cl->constant_pool[cl->this_class])->name_index])->bytes;
        this->constant_pool = cl->constant_pool;
        
        for (uint16_t i = 0; i < cl->fields_count; i++) {
            char* name = (char*)((Constant_Utf8*) constant_pool[cl->fields[i]->name_index])->bytes;
            if (AF_isStatic(cl->fields[i]->access_flags)) {
                this->registerStaticField(name, cl->getStaticFieldSize(name));
            } else {
                this->registerField(name, cl->getFieldSize(name));
            }
        }

        for (uint16_t i = 0; i < cl->methods_count; i++) {
            char* name = (char*)((Constant_Utf8*) constant_pool[cl->methods[i]->name_index])->bytes;
            char* signature = (char*)((Constant_Utf8*) constant_pool[cl->methods[i]->descriptor_index])->bytes;
            MethodInterface* interface = (MethodInterface*) malloc(sizeof(MethodInterface));
            interface->type = 0;
            ByteCodeMethod* bytecode = (ByteCodeMethod*) malloc(sizeof(ByteCodeMethod));
            interface->method = (uintptr_t) bytecode;
            for (uint16_t j = 0; j < cl->methods[i]->attributes_count; i++) {
                Attribute_Code* attr = (Attribute_Code*) cl->methods[i]->attributes[j];
                if (attr->attribute_type == AT_Code) {
                    bytecode->max_stack = attr->max_stack*4;
                    bytecode->max_locals = attr->max_locals*4;
                    bytecode->code = attr->code;
                    bytecode->code_length = attr->code_length;
                    break;
                }
            }
            if (AF_isStatic(cl->methods[i]->access_flags)) {
                this->registerStaticMethod(name, signature, interface);
            } else {
                this->registerMethod(name, signature, interface);
            }
        }

    }

    char* ClassArea::getName() {
        return this->name;
    }

    uintptr_t* ClassArea::getConstantPool() {
        return this->constant_pool;
    }

    uintptr_t ClassArea::newObject() {
        if (fields_size == 0) return NULL;
        return malloc(fields_size);
    }

    uintptr_t ClassArea::newStaticObject() {
        if (static_fields_size == 0) return NULL;
        return malloc(static_fields_size);
    }

    void ClassArea::registerField(char* name, uint8_t size) {
        FieldDescriptor* fd = (FieldDescriptor*) malloc(sizeof(FieldDescriptor));
        fd->size = size;
        fd->index = fields_size;
        fields_size += size;
        map_set(fields_map, name, fd);
    }

    void ClassArea::registerStaticField(char* name, uint8_t size) {
        FieldDescriptor* fd = (FieldDescriptor*) malloc(sizeof(FieldDescriptor));
        fd->size = size;
        fd->index = static_fields_size;
        static_fields_size += size;
        map_set(static_fields_map, name, fd);
    }

    uint8_t ClassArea::getStaticFieldSize(char* name) {
        FieldDescriptor* fd = (FieldDescriptor*) map_get(static_fields_map, name);
        return fd->size;
    }

    uint8_t ClassArea::getFieldSize(char* name) {
        FieldDescriptor* fd = (FieldDescriptor*) map_get(fields_map, name);
        return fd->size;
    }

    void ClassArea::registerMethod(char* name, char* signature, MethodInterface* interface) {
        kot_vector_t* signature_map = (kot_vector_t*) map_get(methods_map, signature);
        if (signature_map == NULL) {
            signature_map = map_create();
            map_set(methods_map, signature, signature_map);
        }
        map_set(signature_map, name, interface);
    }

    void ClassArea::registerStaticMethod(char* name, char* signature, MethodInterface* interface) {
        kot_vector_t* signature_map = (kot_vector_t*) map_get(static_methods_map, signature);
        if (signature_map == NULL) {
            signature_map = map_create();
            map_set(static_methods_map, signature, signature_map);
        }
        map_set(signature_map, name, interface);
    }

    uint32_t ClassArea::runMethod(JavaVM* jvm, uint32_t object, char* name, char* signature, uint32_t* args, uint16_t args_length) {
        kot_vector_t* signature_map = (kot_vector_t*) map_get(methods_map, signature);
        MethodInterface* interface = (MethodInterface*) map_get(signature_map, name);
        if (interface->type == 0) {
            Frame* frame = (Frame*) malloc(sizeof(Frame));
            frame->init(jvm, this, (ByteCodeMethod*) interface->method);
            frame->run(NULL, 0);
            return frame->returnValue;
        } else if (interface->type == 1) {
            return ((NativeMethod*) interface->method)->fn(jvm, object, args, args_length);
        }
        return NULL;
    }

    uint32_t ClassArea::runStaticMethod(JavaVM* jvm, char* name, char* signature, uint32_t* args, uint16_t args_length) {
        kot_vector_t* signature_map = (kot_vector_t*) map_get(static_methods_map, signature);
        MethodInterface* interface = (MethodInterface*) map_get(signature_map, name);
        if (interface->type == 0) {
            Frame* frame = (Frame*) malloc(sizeof(Frame));
            frame->init(jvm, this, (ByteCodeMethod*) interface->method);
            frame->run(NULL, 0);
            return frame->returnValue;
        } else if (interface->type == 1) {
            return ((NativeMethod*) interface->method)->fn(jvm, NULL, args, args_length);
        }
        return NULL;
    }

    bool ClassArea::isMethodStatic(char* name, char* signature) {
        kot_vector_t* signature_map = (kot_vector_t*) map_get(static_methods_map, signature);
        if (signature_map == NULL) { return false; }
        MethodInterface* interface = (MethodInterface*) map_get(signature_map, name);
        if (interface == NULL) {
            return false;
        } else {
            return true;
        }
    }

    uint16_t ClassArea::getMethodArgsLength(char* name, char* signature) {
        kot_vector_t* signature_map = (kot_vector_t*) map_get(methods_map, signature);
        MethodInterface* interface = (MethodInterface*) map_get(signature_map, name);
        return interface->args_length;
    }

    uint16_t ClassArea::getStaticMethodArgsLength(char* name, char* signature) {
        kot_vector_t* signature_map = (kot_vector_t*) map_get(static_methods_map, signature);
        MethodInterface* interface = (MethodInterface*) map_get(signature_map, name);
        return interface->args_length;
    }

    uint8_t ClassArea::getField8(uintptr_t object, char* name) {
        FieldDescriptor* fd = (FieldDescriptor*) map_get(fields_map, name);
        return *(uint8_t*)((uint64_t) object + fd->index);
    }

    uint16_t ClassArea::getField16(uintptr_t object, char* name) {
        FieldDescriptor* fd = (FieldDescriptor*) map_get(fields_map, name);
        return *(uint16_t*)((uint64_t) object + fd->index);
    }
    
    uint32_t ClassArea::getField32(uintptr_t object, char* name) {
        FieldDescriptor* fd = (FieldDescriptor*) map_get(fields_map, name);
        return *(uint32_t*)((uint64_t) object + fd->index);
    }
    
    uint64_t ClassArea::getField64(uintptr_t object, char* name) {
        FieldDescriptor* fd = (FieldDescriptor*) map_get(fields_map, name);
        return *(uint64_t*)((uint64_t) object + fd->index);
    }
    
    void ClassArea::setField8(uintptr_t object, char* name, uint8_t item) {
        FieldDescriptor* fd = (FieldDescriptor*) map_get(fields_map, name);
        *(uint8_t*)((uint64_t) object + fd->index) = item;
    }
    
    void ClassArea::setField16(uintptr_t object, char* name, uint16_t item) {
        FieldDescriptor* fd = (FieldDescriptor*) map_get(fields_map, name);
        *(uint16_t*)((uint64_t) object + fd->index) = item;
    }
    
    void ClassArea::setField32(uintptr_t object, char* name, uint32_t item) {
        FieldDescriptor* fd = (FieldDescriptor*) map_get(fields_map, name);
        *(uint32_t*)((uint64_t) object + fd->index) = item;
    }
    
    void ClassArea::setField64(uintptr_t object, char* name, uint64_t item) {
        FieldDescriptor* fd = (FieldDescriptor*) map_get(fields_map, name);
        *(uint64_t*)((uint64_t) object + fd->index) = item;
    }

    uint8_t ClassArea::getStaticField8(char* name) {
        FieldDescriptor* fd = (FieldDescriptor*) map_get(static_fields_map, name);
        return *(uint8_t*)((uint64_t) static_object + fd->index);
    }

    uint16_t ClassArea::getStaticField16(char* name) {
        FieldDescriptor* fd = (FieldDescriptor*) map_get(static_fields_map, name);
        return *(uint16_t*)((uint64_t) static_object + fd->index);
    }
    
    uint32_t ClassArea::getStaticField32(char* name) {
        FieldDescriptor* fd = (FieldDescriptor*) map_get(static_fields_map, name);
        return *(uint32_t*)((uint64_t) static_object + fd->index);
    }
    
    uint64_t ClassArea::getStaticField64(char* name) {
        FieldDescriptor* fd = (FieldDescriptor*) map_get(static_fields_map, name);
        return *(uint64_t*)((uint64_t) static_object + fd->index);
    }
    
    void ClassArea::setStaticField8(char* name, uint8_t item) {
        FieldDescriptor* fd = (FieldDescriptor*) map_get(static_fields_map, name);
        *(uint8_t*)((uint64_t) static_object + fd->index) = item;
    }
    
    void ClassArea::setStaticField16(char* name, uint16_t item) {
        FieldDescriptor* fd = (FieldDescriptor*) map_get(static_fields_map, name);
        *(uint16_t*)((uint64_t) static_object + fd->index) = item;
    }
    
    void ClassArea::setStaticField32(char* name, uint32_t item) {
        FieldDescriptor* fd = (FieldDescriptor*) map_get(static_fields_map, name);
        *(uint32_t*)((uint64_t) static_object + fd->index) = item;
    }
    
    void ClassArea::setStaticField64(char* name, uint64_t item) {
        FieldDescriptor* fd = (FieldDescriptor*) map_get(static_fields_map, name);
        *(uint64_t*)((uint64_t) static_object + fd->index) = item;
    }
    
    void ClassArea::clinit(JavaVM* jvm) {
        static_object = newStaticObject();
        if (constant_pool != NULL) {
            runStaticMethod(jvm, "<clinit>", "()V", NULL, 0);
        }
    }

}
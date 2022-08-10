#include "loader.h"

#include <kot/cstring.h>
namespace SE8 {

    AttributeInfo_Type ClassLoader::getAttributeType(uint16_t attribute_name_index) {

        ConstantPoolEntry* item = (ConstantPoolEntry*) constant_pool[attribute_name_index];

        switch (item->tag) {
            case CONSTANT_Long:
            case CONSTANT_Float:
            case CONSTANT_Double:
            case CONSTANT_Integer:
            case CONSTANT_String:
                return Attribute_ConstantValue;
            case CONSTANT_Utf8:
                char* bytes = (char*) ((Constant_Utf8*) item)->bytes;
                Printlog(bytes);
                if (bytes == "Code") {

                }
        }

    }

    ClassLoader::ClassLoader(uintptr_t bytes) {

        Reader* reader = (Reader*) malloc(sizeof(Reader));
        reader->buffer = bytes;

        // header

        magic = reader->u4B();

        if (magic != 0xCAFEBABE) {
            // todo: create actual error
            Printlog("invalid file format");
            return;
        }

        minorVersion = reader->u2B();
        majorVersion = reader->u2B();

        if (majorVersion != 52) {
            // todo: create actual error
            Printlog("invalid java version");
            return;
        }

        // constant pool

        constant_pool_count = reader->u2B();

        constant_pool = (uintptr_t*) malloc(constant_pool_count*sizeof(uintptr_t));

        for (uint16_t i = 0; i < constant_pool_count-1; i++) {
            uint8_t tag = reader->u1();
            uintptr_t entry;
            if (tag == CONSTANT_Class) {
                uint16_t name_index = reader->u2B();
                entry = (uintptr_t) malloc(sizeof(Constant_ClassInfo));
                ((Constant_ClassInfo*) entry)->name_index = name_index;
            } else if (tag == CONSTANT_Utf8) {
                uint16_t length = reader->u2B();
                uint8_t* bytes = reader->uB(length);
                entry = (uintptr_t) malloc(sizeof(Constant_Utf8));
                ((Constant_Utf8*) entry)->length = length;
                ((Constant_Utf8*) entry)->bytes = bytes;
            } else if (tag == CONSTANT_NameAndType) {
                uint16_t name_index = reader->u2B();
                uint16_t descriptor_index = reader->u2B();
                entry = (uintptr_t) malloc(sizeof(Constant_NameAndType));
                ((Constant_NameAndType*) entry)->name_index = name_index;
                ((Constant_NameAndType*) entry)->descriptor_index = descriptor_index;
            } else if (tag == CONSTANT_String) {
                uint16_t string_index = reader->u2B();
                entry = (uintptr_t) malloc(sizeof(Constant_String));
                ((Constant_String*) entry)->string_index = string_index;
            } else if (tag == CONSTANT_Integer || tag == CONSTANT_Float) {
                uint32_t bytes = reader->u4B();
                entry = (uintptr_t) malloc(sizeof(Constant_Integer_Float));
                ((Constant_Integer_Float*) entry)->bytes = bytes;
            } else if (tag == CONSTANT_Double || tag == CONSTANT_Long) {
                uint32_t high_bytes = reader->u4B();
                uint32_t low_bytes = reader->u4B();
                entry = (uintptr_t) malloc(sizeof(Constant_Double_Long));
                ((Constant_Double_Long*) entry)->high_bytes = high_bytes;
                ((Constant_Double_Long*) entry)->low_bytes = low_bytes;
            } else if (tag == CONSTANT_Fieldref || tag == CONSTANT_Methodref || tag == CONSTANT_InterfaceMethodref) {
                uint16_t class_index = reader->u2B();
                uint16_t name_and_type_index = reader->u2B();
                entry = (uintptr_t) malloc(sizeof(Constant_RefInfo));
                ((Constant_RefInfo*) entry)->class_index = class_index;
                ((Constant_RefInfo*) entry)->name_and_type_index = name_and_type_index;
            } else if (tag == CONSTANT_MethodHandle) {
                uint8_t reference_kind = reader->u1();
                uint16_t reference_index = reader->u2B();
                entry = (uintptr_t) malloc(sizeof(Constant_MethodHandle));
                ((Constant_MethodHandle*) entry)->reference_kind = reference_kind;
                ((Constant_MethodHandle*) entry)->reference_index = reference_index;
            } else if (tag == CONSTANT_MethodType) {
                uint16_t descriptor_index = reader->u2B();
                entry = (uintptr_t) malloc(sizeof(Constant_Type));
                ((Constant_Type*) entry)->descriptor_index = descriptor_index;
            } else if (tag == CONSTANT_InvokeDynamic) {
                uint16_t bootstrap_method_attr_index = reader->u2B();
                uint16_t name_and_type_index = reader->u2B();
                entry = (uintptr_t) malloc(sizeof(Constant_InvokeDynamic));
                ((Constant_InvokeDynamic*) entry)->bootstrap_method_attr_index = bootstrap_method_attr_index;
                ((Constant_InvokeDynamic*) entry)->name_and_type_index = name_and_type_index;
            } else {
                // todo: create actual error
                Printlog("unsupported constant pool tag");
                return;
            }
            ((ConstantPoolEntry*) entry)->tag = tag;
            constant_pool[i] = entry;
        }   

        access_flags = reader->u2B();
        this_class = reader->u2B();
        super_class = reader->u2B();

        // interfaces

        interfaces_count = reader->u2B();
        interfaces = (uint16_t*) malloc(interfaces_count*2);

        for (uint16_t i = 0; i < interfaces_count; i++) {
            uint16_t index = reader->u2B();
            interfaces[i] = index;
        }

        // fields

        fields_count = reader->u2B();
        fields = (FieldInfo**) malloc(sizeof(FieldInfo*)*fields_count);

        for (uint16_t i = 0; i < fields_count; i++) {
            fields[i] = (FieldInfo*) malloc(sizeof(FieldInfo));
            fields[i]->access_flags = reader->u2B();
            fields[i]->name_index = reader->u2B();
            fields[i]->descriptor_index = reader->u2B();
            fields[i]->attributes_count = reader->u2B();
            fields[i]->attributes = (AttributeInfo**) malloc(sizeof(AttributeInfo*)*fields_count);
            for (uint16_t j = 0; j < fields[i]->attributes_count; j++) {
                uint16_t attribute_name_index = reader->u2B();
                uint32_t attribute_length = reader->u4B();
                if (attribute_length == 0) {
                    fields[i]->attributes[j] = NULL;
                } else {
                    AttributeInfo_Type type = getAttributeType(attribute_name_index);
                    if (type == Attribute_ConstantValue) {
                        fields[i]->attributes[j] = (AttributeInfo*) malloc(sizeof(AttributeInfo_ConstantValue));
                        ((AttributeInfo_ConstantValue*) fields[i]->attributes[j])->constantvalue_index = reader->u2B();
                    }
                    fields[i]->attributes[j]->attribute_name_index = attribute_name_index;
                    fields[i]->attributes[j]->type = type;
                }
            }
        }

        // methods

        methods_count = reader->u2B();
        methods = (MethodInfo**) malloc(sizeof(MethodInfo*)*methods_count);

        // attributes

        // attributes_count = reader->u2B();
        // attributes = (SE8AttributeInfo**) malloc(sizeof(SE8AttributeInfo*)*attributes_count);
        // for (uint16_t i = 0; i < attributes_count; i++) {
        //     attributes[i] = (SE8AttributeInfo*) malloc(sizeof(SE8AttributeInfo));
        //     attributes[i]->attribute_name_index = reader->u2B();
        //     attributes[i]->attribute_length = reader->u4B();
        //     if (attributes[i]->attribute_length == 0) {
        //         attributes[i]->info = NULL;
        //     } else {
        //         attributes[i]->info = getAttributesInfo(attributes[i]->attribute_name_index, reader->uB(attributes[i]->attribute_length));
        //     }
        // }

        // free(reader);

    }

    /**
     * @return uint8_t* (utf8 as byte array)
     */
    uint8_t* ClassLoader::getClassName() {
        return ((Constant_Utf8*) this->constant_pool[((Constant_ClassInfo*) this->constant_pool[this->this_class])->name_index])->bytes;
    }

    /**
     * @return uint8_t* (utf8 as byte array)
     */
    uint8_t* ClassLoader::getSuperClassName() {
        return ((Constant_Utf8*) this->constant_pool[((Constant_ClassInfo*) this->constant_pool[this->super_class])->name_index])->bytes;
    }

    uintptr_t* ClassLoader::getConstantPool() {
        return this->constant_pool;
    }

}
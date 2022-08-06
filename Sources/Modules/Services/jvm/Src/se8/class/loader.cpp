#include "loader.h"

#include <kot/cstring.h>

SE8AttributeInfo_Type SE8ClassLoader::getAttributeType(uint16_t attribute_name_index) {

    SE8ConstantPoolEntry* item = (SE8ConstantPoolEntry*) constant_pool[attribute_name_index];

    switch (item->tag) {
        case CONSTANT_Long:
        case CONSTANT_Float:
        case CONSTANT_Double:
        case CONSTANT_Integer:
        case CONSTANT_String:
            return Attribute_ConstantValue;
        case CONSTANT_Utf8:
            char* bytes = (char*) ((SE8Constant_Utf8*) item)->bytes;
            Printlog(bytes);
            if (bytes == "Code") {

            }
    }

}

SE8ClassLoader::SE8ClassLoader(uintptr_t buffer) {

    Reader* reader = (Reader*) malloc(sizeof(Reader));
    reader->buffer = buffer;

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
            entry = (uintptr_t) malloc(sizeof(SE8Constant_ClassInfo));
            ((SE8Constant_ClassInfo*) entry)->name_index = name_index;
        } else if (tag == CONSTANT_Utf8) {
            uint16_t length = reader->u2B();
            uint8_t* bytes = reader->uB(length);
            entry = (uintptr_t) malloc(sizeof(SE8Constant_Utf8));
            ((SE8Constant_Utf8*) entry)->length = length;
            ((SE8Constant_Utf8*) entry)->bytes = bytes;
        } else if (tag == CONSTANT_NameAndType) {
            uint16_t name_index = reader->u2B();
            uint16_t descriptor_index = reader->u2B();
            entry = (uintptr_t) malloc(sizeof(SE8Constant_NameAndType));
            ((SE8Constant_NameAndType*) entry)->name_index = name_index;
            ((SE8Constant_NameAndType*) entry)->descriptor_index = descriptor_index;
        } else if (tag == CONSTANT_String) {
            uint16_t string_index = reader->u2B();
            entry = (uintptr_t) malloc(sizeof(SE8Constant_String));
            ((SE8Constant_String*) entry)->string_index = string_index;
        } else if (tag == CONSTANT_Integer || tag == CONSTANT_Float) {
            uint32_t bytes = reader->u4B();
            entry = (uintptr_t) malloc(sizeof(SE8Constant_Integer_Float));
            ((SE8Constant_Integer_Float*) entry)->bytes = bytes;
        } else if (tag == CONSTANT_Double || tag == CONSTANT_Long) {
            uint32_t high_bytes = reader->u4B();
            uint32_t low_bytes = reader->u4B();
            entry = (uintptr_t) malloc(sizeof(SE8Constant_Double_Long));
            ((SE8Constant_Double_Long*) entry)->high_bytes = high_bytes;
            ((SE8Constant_Double_Long*) entry)->low_bytes = low_bytes;
        } else if (tag == CONSTANT_Fieldref || tag == CONSTANT_Methodref || tag == CONSTANT_InterfaceMethodref) {
            uint16_t class_index = reader->u2B();
            uint16_t name_and_type_index = reader->u2B();
            entry = (uintptr_t) malloc(sizeof(SE8Constant_RefInfo));
            ((SE8Constant_RefInfo*) entry)->class_index = class_index;
            ((SE8Constant_RefInfo*) entry)->name_and_type_index = name_and_type_index;
        } else if (tag == CONSTANT_MethodHandle) {
            uint8_t reference_kind = reader->u1();
            uint16_t reference_index = reader->u2B();
            entry = (uintptr_t) malloc(sizeof(SE8Constant_MethodHandle));
            ((SE8Constant_MethodHandle*) entry)->reference_kind = reference_kind;
            ((SE8Constant_MethodHandle*) entry)->reference_index = reference_index;
        } else if (tag == CONSTANT_MethodType) {
            uint16_t descriptor_index = reader->u2B();
            entry = (uintptr_t) malloc(sizeof(SE8Constant_Type));
            ((SE8Constant_Type*) entry)->descriptor_index = descriptor_index;
        } else if (tag == CONSTANT_InvokeDynamic) {
            uint16_t bootstrap_method_attr_index = reader->u2B();
            uint16_t name_and_type_index = reader->u2B();
            entry = (uintptr_t) malloc(sizeof(SE8Constant_InvokeDynamic));
            ((SE8Constant_InvokeDynamic*) entry)->bootstrap_method_attr_index = bootstrap_method_attr_index;
            ((SE8Constant_InvokeDynamic*) entry)->name_and_type_index = name_and_type_index;
        } else {
            // todo: create actual error
            Printlog("unsupported constant pool tag");
            return;
        }
        ((SE8ConstantPoolEntry*) entry)->tag = tag;
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
    fields = (SE8FieldInfo**) malloc(sizeof(SE8FieldInfo*)*fields_count);

    for (uint16_t i = 0; i < fields_count; i++) {
        fields[i] = (SE8FieldInfo*) malloc(sizeof(SE8FieldInfo));
        fields[i]->access_flags = reader->u2B();
        fields[i]->name_index = reader->u2B();
        fields[i]->descriptor_index = reader->u2B();
        fields[i]->attributes_count = reader->u2B();
        fields[i]->attributes = (SE8AttributeInfo**) malloc(sizeof(SE8AttributeInfo*)*fields_count);
        for (uint16_t j = 0; j < fields[i]->attributes_count; j++) {
            uint16_t attribute_name_index = reader->u2B();
            uint32_t attribute_length = reader->u4B();
            if (attribute_length == 0) {
                fields[i]->attributes[j] = NULL;
            } else {
                SE8AttributeInfo_Type type = getAttributeType(attribute_name_index);
                if (type == Attribute_ConstantValue) {
                    fields[i]->attributes[j] = (SE8AttributeInfo*) malloc(sizeof(SE8AttributeInfo_ConstantValue));
                    ((SE8AttributeInfo_ConstantValue*) fields[i]->attributes[j])->constantvalue_index = reader->u2B();
                }
                fields[i]->attributes[j]->attribute_name_index = attribute_name_index;
                fields[i]->attributes[j]->type = type;
            }
        }
    }

    // methods

    methods_count = reader->u2B();
    methods = (SE8MethodInfo**) malloc(sizeof(SE8MethodInfo*)*methods_count);

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

char* SE8ClassLoader::getName() {
    return "";
}
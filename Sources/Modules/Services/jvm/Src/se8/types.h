#pragma once

#include <kot/heap.h>
#include <kot/cstring.h>

#include "../utils/reader.h"

namespace SE8 {
    
    enum Types : uint8_t {
        Null = 0,
        NaN = 1,
        Short = 2,
        Int = 3,
        Long = 4,
        Float = 5,
        Double = 6,
        Char = 7,
        Byte = 8,
        ArrayRef = 9,
    };

    enum ArrayTypes : uint8_t {
        AT_BOOLEAN = 4,
        AT_CHAR	= 5,
        AT_FLOAT = 6,
        AT_DOUBLE = 7,
        AT_BYTE	= 8,
        AT_SHORT = 9,
        AT_INT = 10,
        AT_LONG = 11
    };

    struct Value {
        uint8_t type;
        uint8_t bytes[];
    };

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

    enum AttributeType {
        AT_ConstantValue,
        AT_Code,
        AT_StackMapTable,
        AT_Exceptions,
        AT_InnerClasses,
        AT_EnclosingMethod,
        AT_Signature,
        AT_SourceFile,
        AT_SourceDebugExtension,
        AT_LineNumberTable,
        AT_Deprecated,
        AT_Synthetic,
    };

    struct Attribute {
        uint16_t attribute_name_index;
        uint32_t attribute_length;
        uint8_t attribute_type;
    };

    struct Attribute_ConstantValue {
        uint16_t attribute_name_index;
        uint32_t attribute_length;
        uint8_t attribute_type;
        uint16_t constantvalue_index;
    };

    struct ExceptionTable {
        uint16_t start_pc;
        uint16_t end_pc;
        uint16_t handler_pc;
        uint16_t catch_type;
    };

    struct Attribute_Code {
        uint16_t attribute_name_index;
        uint32_t attribute_length;
        uint8_t attribute_type;
        uint16_t max_stack;
        uint16_t max_locals;
        uint32_t code_length;
        uint8_t* code;
        uint16_t exception_length;
        ExceptionTable** exception_table;
        uint16_t attributes_count;
        Attribute** attributes;
    };

    struct Top_variable_info {
        uint8_t tag; /* 0 */
    };

    struct Integer_variable_info {
        uint8_t tag; /* 1 */
    };

    struct Float_variable_info {
        uint8_t tag; /* 2 */
    };

    struct Null_variable_info {
        uint8_t tag; /* 5 */
    };

    struct UninitializedThis_variable_info  {
        uint8_t tag; /* 6 */
    };

    struct Object_variable_info  {
        uint8_t tag; /* 7 */
        uint16_t cpool_index;
    };

    struct Uninitialized_variable_info  {
        uint8_t tag; /* 8 */
        uint16_t offset;
    };

    struct Long_variable_info {
        uint8_t tag; /* 4 */
    };

    struct Double_variable_info {
        uint8_t tag; /* 3 */
    };

    union verification_type_info {
        Top_variable_info top_variable_info;
        Integer_variable_info integer_variable_info;
        Float_variable_info float_variable_info;
        Long_variable_info long_variable_info;
        Double_variable_info double_variable_info;
        Null_variable_info null_variable_info;
        UninitializedThis_variable_info uninitializedThis_variable_info;
        Object_variable_info object_variable_info;
        Uninitialized_variable_info uninitialized_variable_info;
    };

    struct SameFrame {
        uint8_t frame_type; /* 0-63 */
    };

    struct SameLocals1StackItemFrame {
        uint8_t frame_type; /* 64-127 */
        verification_type_info* stack;
    };

    struct SameLocals1StackItemFrameExtended {
        uint8_t frame_type; /* 247 */
        uint16_t offset_delta;
        verification_type_info* stack;
    };

    struct ChopFrame {
        uint8_t frame_type; /* 248-250 */
        uint16_t offset_delta;
    };

    struct SameFrameExtended {
        uint8_t frame_type; /* 251 */
        uint16_t offset_delta;
    };

    struct AppendFrame {
        uint8_t frame_type; /* 252-254 */
        uint16_t offset_delta;
        verification_type_info** stack; /* frame_type - 251 */
    };

    struct FullFrame {
        uint8_t frame_type; /* 255 */
        uint16_t offset_delta;
        uint16_t number_of_locals;
        verification_type_info** locals;
        uint16_t number_of_stack_items;
        verification_type_info** stack;
    };

    union StackMapFrame {
        SameFrame same_frame;
        SameLocals1StackItemFrame same_locals_1_stack_item_frame;
        SameLocals1StackItemFrameExtended same_locals_1_stack_item_frame_extended;
        ChopFrame chop_frame;
        SameFrameExtended same_frame_extended;
        AppendFrame append_frame;
        FullFrame full_frame;
    };

    struct Attribute_StackMapTable {
        uint16_t attribute_name_index;
        uint32_t attribute_length;
        uint8_t attribute_type;
        uint16_t number_of_entries;
        StackMapFrame** entries;
    };

    struct Attribute_Exceptions {
        uint16_t attribute_name_index;
        uint32_t attribute_length;
        uint8_t attribute_type;
        uint16_t number_of_exceptions;
        uint16_t* exception_index_table;
    };

    struct InnerClass {
        uint16_t inner_class_info_index;
        uint16_t outer_class_info_index;
        uint16_t inner_name_index;
        uint16_t inner_class_access_flags;
    };

    struct Attribute_InnerClasses {
        uint16_t attribute_name_index;
        uint32_t attribute_length;
        uint8_t attribute_type;
        uint16_t number_of_classes;
        InnerClass* classes;
    };

    struct Attribute_EnclosingMethod {
        uint16_t attribute_name_index;
        uint32_t attribute_length;
        uint8_t attribute_type;
        uint16_t class_index;
        uint16_t method_index;
    };

    struct Attribute_Signature {
        uint16_t attribute_name_index;
        uint32_t attribute_length;
        uint8_t attribute_type;
        uint16_t signature_index;
    };

    struct Attribute_SourceFile {
        uint16_t attribute_name_index;
        uint32_t attribute_length;
        uint8_t attribute_type;
        uint16_t sourcefile_index;
    };

    struct Attribute_SourceDebugExtension {
        uint16_t attribute_name_index;
        uint32_t attribute_length;
        uint8_t attribute_type;
        uint8_t* debug_extension;
    };

    struct LineNumberTable {
        uint16_t start_pc;
        uint16_t line_number;
    };

    struct Attribute_LineNumberTable {
        uint16_t attribute_name_index;
        uint32_t attribute_length;
        uint8_t attribute_type;
        uint16_t line_number_table_length;
        LineNumberTable* line_number_table;
    };

    struct FieldInfo {
        uint16_t access_flags;
        uint16_t name_index;
        uint16_t descriptor_index;
        uint16_t attributes_count;
        Attribute** attributes;
    };

    struct Method {
        uint16_t access_flags;
        uint16_t name_index;
        uint16_t descriptor_index;
        uint16_t attributes_count;
        Attribute** attributes;
    };

    enum OpCodes {
        aaload = 0x32,
        aastore = 0x53,
        aconst_null = 0x1,
        aload = 0x19,
        aload_0 = 0x2a,
        aload_1 = 0x2b,
        aload_2 = 0x2c,
        aload_3 = 0x2d,
        anewarray = 0xbd,
        areturn = 0xb0,
        arraylength = 0xbe,
        astore = 0x3a,
        astore_0 = 0x4b,
        astore_1 = 0x4c,
        astore_2 = 0x4d,
        astore_3 = 0x4e,
        athrow = 0xbf,
        baload = 0x33,
        bastore = 0x54,
        bipush = 0x10,
        caload = 0x34,
        castore = 0x55,
        checkcast = 0xc0,
        d2f = 0x90,
        d2i = 0x8e,
        d2l = 0x8f,
        dadd = 0x63,
        daload = 0x31,
        dastore = 0x52,
        dcmpg = 0x98,
        dcmpl = 0x97,
        dconst_0 = 0xe,
        dconst_1 = 0xf,
        ddiv = 0x6f,
        dload = 0x18,
        dload_0 = 0x26,
        dload_1 = 0x27,
        dload_2 = 0x28,
        dload_3 = 0x29,
        dmul = 0x6b,
        dneg = 0x77,
        drem = 0x73,
        dreturn = 0xaf,
        dstore = 0x39,
        dstore_0 = 0x47,
        dstore_1 = 0x48, 
        dstore_2 = 0x49,
        dstore_3 = 0x4a,
        dsub = 0x67,
        dup = 0x59,
        dup_x1 = 0x5a,
        dup_x2 = 0x5b,
        dup2 = 0x5c,
        dup2_x1 = 0x5d,
        dup2_x2 = 0x5e,
        f2d = 0x8d,
        f2i = 0x8b,
        f2l = 0x8c,
        fadd = 0x62,
        faload = 0x30,
        fastore = 0x51,
        fcmpg = 0x96,
        fcpml = 0x95,
        fconst_0 = 0xb,
        fconst_1 = 0xc,
        fconst_2 = 0xd,
        fdiv = 0x6e,
        fload = 0x17,
        fload_0 = 0x22,
        fload_1 = 0x23,
        fload_2 = 0x24,
        fload_3 = 0x25,
        fmul = 0x6a,
        fneg = 0x76,
        frem = 0x72,
        freturn = 0xae,
        fstore = 0x38,
        fstore_0 = 0x43,
        fstore_1 = 0x44,
        fstore_2 = 0x45,
        fstore_3 = 0x46,
        fsub = 0x66,
        getfield = 0xb4,
        getstatic = 0xb2,
        goto_ = 0xa7,
        goto_w = 0xc8,
        i2b = 0x91,
        i2c = 0x92,
        i2d = 0x87,
        i2f = 0x86,
        i2l = 0x85,
        i2s = 0x93,
        iadd = 0x60,
        iaload = 0x2e,
        iand = 0x7e,
        iastore = 0x4f,
        iconst_m1 = 0x2,
        iconst_0 = 0x3,
        iconst_1 = 0x4,
        iconst_2 = 0x5,
        iconst_3 = 0x6,
        iconst_4 = 0x7,
        iconst_5 = 0x8,
        idiv = 0x6c,
        if_acmpeq = 0xa5,
        if_acmpne = 0xa6,
        if_icmpeq = 0x9f,
        if_icmpne = 0xa0,
        if_icmplt = 0xa1,
        if_icmpge = 0xa2,
        if_icmpgt = 0xa3,
        if_icmple = 0xa4,
        ifeq = 0x99,
        ifne = 0x9a,
        iflt = 0x9b,
        ifge = 0x9c,
        ifgt = 0x9d,
        ifle = 0x9e,
        ifnonnull = 0xc7,
        ifnull = 0xc6,
        iinc = 0x84,
        iload = 0x15,
        iload_0 = 0x1a,
        iload_1 = 0x1b,
        iload_2 = 0x1c,
        iload_3 = 0x1d,
        imul = 0x68,
        ineg = 0x74,
        instanceof = 0xc1,
        invokedynamic = 0xba,
        invokeinterface = 0xb9,
        invokespecial = 0xb7,
        invokestatic = 0xb8,
        invokevirtual = 0xb6,
        ior = 0x80,
        irem = 0x70,
        ireturn = 0xac,
        ishl = 0x78,
        ishr = 0x7a,
        istore = 0x36,
        istore_0 = 0x3b,
        istore_1 = 0x3c,
        istore_2 = 0x3d,
        istore_3 = 0x3e,
        isub = 0x64,
        iushr = 0x7c,
        ixor = 0x82,
        jsr = 0xa8,
        jsr_w = 0xc9,
        l2d = 0x8a,
        l2f = 0x89,
        l2i = 0x88,
        ladd = 0x61,
        laload = 0x2f,
        land = 0x7f,
        lastore = 0x50,
        lcmp = 0x94,
        lconst_0 = 0x9,
        lconst_1 = 0xa,
        ldc = 0x12,
        ldc_w = 0x13,
        ldc2_w = 0x14,
        ldiv = 0x6d,
        lload = 0x16,
        lload_0 = 0x1e,
        lload_1 = 0x1f,
        lload_2 = 0x20,
        lload_3 = 0x21,
        lmul = 0x69,
        lneg = 0x75,
        lookupswitch = 0xab,
        lor = 0x81,
        lrem = 0x71,
        lreturn = 0xad,
        lshl = 0x79,
        lshr = 0x7b,
        lstore = 0x37,
        lstore_0 = 0x3f,
        lstore_1 = 0x40,
        lstore_2 = 0x41,
        lstore_3 = 0x42,
        lsub = 0x65,
        lushr = 0x7d,
        lxor = 0x83,
        monitorenter = 0xc2,
        monitorexit = 0xc3,
        multianewarray = 0xc5,
        new_ = 0xbb,
        newarray = 0xbc,
        nop = 0x0,
        pop = 0x57,
        pop2 = 0x58,
        putfield = 0xb5,
        putstatic = 0xb3,
        ret = 0xa9,
        return_ = 0xb1,
        saload = 0x35,
        sastore = 0x56,
        sipush = 0x11,
        swap = 0x5f,
        tableswitch = 0xaa,
        wide = 0xc4,
    };

    inline uint8_t getTypeSize(uint8_t type) {
        switch (type) {
            case SE8::Null:
            case SE8::NaN:
                return 0;
            case SE8::Byte:
                return 1;
            case SE8::Char:
            case SE8::Short:
                return 2;
            case SE8::Int:
            case SE8::Float:
                return 4;
            case SE8::Long:
            case SE8::Double:
            case SE8::ArrayRef:
                return 8;
        }
    }

    inline bool AF_check(uint16_t flags, AccessFlags flag) {
        return ((flags & flag) == flag);
    }

    inline bool AF_isStatic(uint16_t flags) {
        return AF_check(flags, AccessFlags::ACC_STATIC);
    }

    inline bool AF_isPublic(uint16_t flags) {
        return AF_check(flags, AccessFlags::ACC_PUBLIC);
    }

}
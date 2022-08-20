#include "frame.h"

// todo: implement float & double (IEEE 754)

namespace SE8 {

    void Frame::init(JavaVM* jvm, ClassArea* cl, ByteCodeMethod* code) {
        constant_pool = cl->getConstantPool();
        this->code = code;
        this->jvm = jvm;
        reader = (Reader*) malloc(sizeof(Reader));
        stack = new std::Stack(code->max_stack);
        locals = malloc(code->max_locals);
        reader->buffer = code->code;
        code_length = code->code_length;
    }

    void Opc::aaload(Frame* frame) {

    }

    void Opc::aastore(Frame* frame) {

    }

    void Opc::aconst_null(Frame* frame) {
        frame->stack->push32(NULL);
    }

    void Opc::aload(Frame* frame) {
        uint16_t idx = frame->widened ? frame->reader->u2B() : frame->reader->u1();
        frame->stack->push32(*(uint32_t*)((uint64_t) frame->locals + idx * 4));
        frame->widened = false;
    }

    void Opc::aload_0(Frame* frame) {
        frame->stack->push32(*(uint32_t*)((uint64_t) frame->locals));
    }

    void Opc::aload_1(Frame* frame) {
        frame->stack->push32(*(uint32_t*)((uint64_t) frame->locals + 1 * 4));
    }

    void Opc::aload_2(Frame* frame) {
        frame->stack->push32(*(uint32_t*)((uint64_t) frame->locals + 2 * 4));
    }

    void Opc::aload_3(Frame* frame) {
        frame->stack->push32(*(uint32_t*)((uint64_t) frame->locals + 3 * 4));
    }

    void Opc::anewarray(Frame* frame) { 

    }

    void Opc::areturn(Frame* frame) {
        frame->returnValue = frame->stack->pop64();
        frame->code_length = 0;
    }

    void Opc::arraylength(Frame* frame) {

    }

    void Opc::astore(Frame* frame) {
        uint16_t idx = frame->widened ? frame->reader->u2B() : frame->reader->u1();
        *(uint32_t*)((uint64_t) frame->locals + idx * 4) = frame->stack->pop32();
        frame->widened = false;
    }

    void Opc::astore_0(Frame* frame) {
        *(uint32_t*)((uint64_t) frame->locals) = frame->stack->pop32();
    }

    void Opc::astore_1(Frame* frame) {
        *(uint32_t*)((uint64_t) frame->locals + 1 * 4) = frame->stack->pop32();
    }

    void Opc::astore_2(Frame* frame) {
        *(uint32_t*)((uint64_t) frame->locals + 2 * 4) = frame->stack->pop32();
    }

    void Opc::astore_3(Frame* frame) {
        *(uint32_t*)((uint64_t) frame->locals + 3 * 4) = frame->stack->pop32();
    }

    void Opc::athrow(Frame* frame) {

    }

    void Opc::baload(Frame* frame) {
        int32_t index = frame->stack->pop32();
        uint32_t arrayref = frame->stack->pop32();
        vector_t* rs = frame->jvm->getRefSys();
        uintptr_t arr = vector_get(rs, arrayref);
        frame->stack->push32(*(uint8_t*)((uint64_t) arr + index));
    }

    void Opc::bastore(Frame* frame) {
        uint32_t value = frame->stack->pop32();
        int32_t index = frame->stack->pop32();
        uint32_t arrayref = frame->stack->pop32();
        vector_t* rs = frame->jvm->getRefSys();
        uintptr_t arr = vector_get(rs, arrayref);
        *(uint8_t*)((uint64_t) arr + index) = (uint8_t) value;
    }

    void Opc::bipush(Frame* frame) {
        frame->stack->push32(frame->reader->u1());
    }

    void Opc::caload(Frame* frame) {

    }

    void Opc::castore(Frame* frame) {
        uint32_t value = frame->stack->pop32();
        int32_t index = frame->stack->pop32();
        uint32_t arrayref = frame->stack->pop32();
        vector_t* rs = frame->jvm->getRefSys();
        uintptr_t arr = vector_get(rs, arrayref);
        *(uint16_t*)((uint64_t) arr + index * 2) = (uint16_t) value;
    }

    void Opc::checkcast(Frame* frame) {

    }

    void Opc::d2f(Frame* frame) {

    }

    void Opc::d2i(Frame* frame) {
 
    }

    void Opc::d2l(Frame* frame) {

    }

    void Opc::dadd(Frame* frame) {

    }

    void Opc::daload(Frame* frame) {

    }

    void Opc::dastore(Frame* frame) {
        
    }

    void Opc::dcmpg(Frame* frame) {

    }

    void Opc::dcmpl(Frame* frame) {
        
    }

    void Opc::dconst_0(Frame* frame) {
        
    }

    void Opc::dconst_1(Frame* frame) {
        
    }

    void Opc::ddiv(Frame* frame) {
        
    }

    void Opc::dload(Frame* frame) {
        
    }

    void Opc::dload_0(Frame* frame) {
        
    }

    void Opc::dload_1(Frame* frame) {
        
    }

    void Opc::dload_2(Frame* frame) {
        
    }

    void Opc::dload_3(Frame* frame) {
        
    }

    void Opc::dmul(Frame* frame) {

    }

    void Opc::dneg(Frame* frame) {

    }

    void Opc::drem(Frame* frame) {

    }

    void Opc::dreturn(Frame* frame) {
        frame->code_length = 0;
    }   

    void Opc::dstore(Frame* frame) {

    }

    void Opc::dstore_0(Frame* frame) {

    }

    void Opc::dstore_1(Frame* frame) {

    }

    void Opc::dstore_2(Frame* frame) {

    }

    void Opc::dstore_3(Frame* frame) {

    }

    void Opc::dsub(Frame* frame) {

    }

    void Opc::dup(Frame* frame) {
        frame->stack->push32(frame->stack->peek32());
    }

    void Opc::dup_x1(Frame* frame) {
        uint32_t val1 = frame->stack->pop32();
        uint32_t val2 = frame->stack->pop32();
        frame->stack->push32(val1);
        frame->stack->push32(val2);
        frame->stack->push32(val1);
    }

    void Opc::dup_x2(Frame* frame) {
        uint32_t val1 = frame->stack->pop32();
        uint32_t val2 = frame->stack->pop32();
        uint32_t val3 = frame->stack->pop32();
        frame->stack->push32(val1);
        frame->stack->push32(val3);
        frame->stack->push32(val2);
        frame->stack->push32(val1);
    }

    void Opc::dup2(Frame* frame) {
        frame->stack->push64(frame->stack->peek64());
    }

    void Opc::dup2_x1(Frame* frame) {
        uint32_t val1 = frame->stack->pop64();
        uint32_t val2 = frame->stack->pop64();
        frame->stack->push64(val1);
        frame->stack->push64(val2);
        frame->stack->push64(val1);
    }

    void Opc::dup2_x2(Frame* frame) {
        uint32_t val1 = frame->stack->pop64();
        uint32_t val2 = frame->stack->pop64();
        uint32_t val3 = frame->stack->pop64();
        frame->stack->push64(val1);
        frame->stack->push64(val3);
        frame->stack->push64(val2);
        frame->stack->push64(val1);
    }

    void Opc::f2d(Frame* frame) {

    }

    void Opc::f2i(Frame* frame) {

    }

    void Opc::f2l(Frame* frame) {

    }

    void Opc::fadd(Frame* frame) {

    }

    void Opc::faload(Frame* frame) {

    }

    void Opc::fastore(Frame* frame) {

    }

    void Opc::fcmpg(Frame* frame) {

    }

    void Opc::fcpml(Frame* frame) {

    }

    void Opc::fconst_0(Frame* frame) {

    }

    void Opc::fconst_1(Frame* frame) {

    }

    void Opc::fconst_2(Frame* frame) {

    }

    void Opc::fdiv(Frame* frame) {

    }

    void Opc::fload(Frame* frame) {

    }

    void Opc::fload_0(Frame* frame) {

    }

    void Opc::fload_1(Frame* frame) {

    }

    void Opc::fload_2(Frame* frame) {

    }

    void Opc::fload_3(Frame* frame) {

    }

    void Opc::fmul(Frame* frame) {

    }

    void Opc::fneg(Frame* frame) {

    }

    void Opc::frem(Frame* frame) {

    }

    void Opc::freturn(Frame* frame) {
        frame->code_length = 0;
    }

    void Opc::fstore(Frame* frame) {

    }

    void Opc::fstore_0(Frame* frame) {

    }

    void Opc::fstore_1(Frame* frame) {

    }

    void Opc::fstore_2(Frame* frame) {

    }

    void Opc::fstore_3(Frame* frame) {

    }

    void Opc::fsub(Frame* frame) {

    }

    void Opc::getfield(Frame* frame) {

    }

    void Opc::getstatic(Frame* frame) {
        uint16_t idx = frame->reader->u2B();
        Constant_RefInfo* field = (Constant_RefInfo*) frame->constant_pool[idx];
        char* className = (char*) ((Constant_Utf8*) frame->constant_pool[((Constant_ClassInfo*) frame->constant_pool[field->class_index])->name_index])->bytes;
        char* fieldName = (char*) ((Constant_Utf8*) frame->constant_pool[((Constant_NameAndType*) frame->constant_pool[field->name_and_type_index])->name_index])->bytes;
        ClassArea* cl = frame->jvm->getClasses()->getClass(className);
        uint8_t fieldSize = cl->getStaticFieldSize(fieldName);
        switch (fieldSize) {
            case 1:
                frame->stack->push32(cl->getStaticField8(fieldName));
                break;
            case 2:
                frame->stack->push32(cl->getStaticField16(fieldName));
                break;
            case 4:
                frame->stack->push32(cl->getStaticField32(fieldName));
                break;
            case 8:
                frame->stack->push64(cl->getStaticField64(fieldName));
                break;
        }
    }

    void Opc::goto_(Frame* frame) {
        frame->reader->index += frame->reader->u2B();
    }

    void Opc::goto_w(Frame* frame) {
        frame->reader->index += frame->reader->u4B();
    }

    void Opc::i2b(Frame* frame) {
        int32_t val = frame->stack->pop32();
        frame->stack->push32(static_cast<uint8_t>(val));
    }

    void Opc::i2c(Frame* frame) {
        int32_t val = frame->stack->pop32();
        frame->stack->push32(static_cast<uint16_t>(val));
    }

    void Opc::i2d(Frame* frame) {

    }

    void Opc::i2f(Frame* frame) {

    }

    void Opc::i2l(Frame* frame) {
        int32_t val = frame->stack->pop32();
        frame->stack->push64(static_cast<int64_t>(val));
    }

    void Opc::i2s(Frame* frame) {
        int32_t val = frame->stack->pop32();
        frame->stack->push32(static_cast<int16_t>(val));
    }

    void Opc::iadd(Frame* frame) {
        int32_t value2 = frame->stack->pop32();
        int32_t value1 = frame->stack->pop32();
        frame->stack->push32(value1 + value2);
    }

    void Opc::iaload(Frame* frame) {
        int32_t index = frame->stack->pop32();
        uint32_t arrayref = frame->stack->pop32();
        vector_t* rs = frame->jvm->getRefSys();
        uintptr_t arr = vector_get(rs, arrayref);
        frame->stack->push32(*(uint32_t*)((uint64_t) arr + index * 4));
    }

    void Opc::iand(Frame* frame) {
        int32_t value2 = frame->stack->pop32();
        int32_t value1 = frame->stack->pop32();
        frame->stack->push32(value1 & value2);
    }

    void Opc::iastore(Frame* frame) {
        uint32_t value = frame->stack->pop32();
        int32_t index = frame->stack->pop32();
        uint32_t arrayref = frame->stack->pop32();
        vector_t* rs = frame->jvm->getRefSys();
        uintptr_t arr = vector_get(rs, arrayref);
        *(uint32_t*)((uint64_t) arr + index * 4) = value;
    }

    void Opc::iconst_m1(Frame* frame) {
        frame->stack->push32(-1);
    }

    void Opc::iconst_0(Frame* frame) {
        frame->stack->push32(0);
    }

    void Opc::iconst_1(Frame* frame) {
        frame->stack->push32(1);
    }

    void Opc::iconst_2(Frame* frame) {
        frame->stack->push32(2);
    }

    void Opc::iconst_3(Frame* frame) {
        frame->stack->push32(3);
    }

    void Opc::iconst_4(Frame* frame) {
        frame->stack->push32(4);
    }

    void Opc::iconst_5(Frame* frame) {
        frame->stack->push32(5);
    }

    void Opc::idiv(Frame* frame) {
        int32_t value2 = frame->stack->pop32();
        int32_t value1 = frame->stack->pop32();
        if (value2 == 0) {
            // throw arithmetic exception
        }
        frame->stack->push32(value1 / value2);
    }

    void Opc::if_acmpeq(Frame* frame) {
        uint32_t idx = frame->reader->u2B();
        uint32_t value2 = frame->stack->pop32();
        uint32_t value1 = frame->stack->pop32();
        if (value1 == value2) {
            frame->reader->index += idx; 
        }
    }

    void Opc::if_acmpne(Frame* frame) {
        uint32_t idx = frame->reader->u2B();
        uint32_t value2 = frame->stack->pop32();
        uint32_t value1 = frame->stack->pop32();
        if (value1 != value2) {
            frame->reader->index += idx; 
        }
    }

    void Opc::if_icmpeq(Frame* frame) {
        uint32_t idx = frame->reader->u2B();
        int32_t value2 = frame->stack->pop32();
        int32_t value1 = frame->stack->pop32();
        if (value1 == value2) {
            frame->reader->index += idx; 
        }
    }

    void Opc::if_icmpne(Frame* frame) {
        uint32_t idx = frame->reader->u2B();
        int32_t value2 = frame->stack->pop32();
        int32_t value1 = frame->stack->pop32();
        if (value1 != value2) {
            frame->reader->index += idx; 
        }
    }

    void Opc::if_icmplt(Frame* frame) {
        uint32_t idx = frame->reader->u2B();
        int32_t value2 = frame->stack->pop32();
        int32_t value1 = frame->stack->pop32();
        if (value1 < value2) {
            frame->reader->index += idx; 
        }
    }

    void Opc::if_icmpge(Frame* frame) {
        uint32_t idx = frame->reader->u2B();
        int32_t value2 = frame->stack->pop32();
        int32_t value1 = frame->stack->pop32();
        if (value1 >= value2) {
            frame->reader->index += idx; 
        }
    }

    void Opc::if_icmpgt(Frame* frame) {
        uint32_t idx = frame->reader->u2B();
        int32_t value2 = frame->stack->pop32();
        int32_t value1 = frame->stack->pop32();
        if (value1 > value2) {
            frame->reader->index += idx; 
        }
    }

    void Opc::if_icmple(Frame* frame) {
        uint32_t idx = frame->reader->u2B();
        int32_t value2 = frame->stack->pop32();
        int32_t value1 = frame->stack->pop32();
        if (value1 <= value2) {
            frame->reader->index += idx; 
        }
    }

    void Opc::ifeq(Frame* frame) {
        uint32_t idx = frame->reader->u2B();
        int32_t value = frame->stack->pop32();
        if (value == 0) {
            frame->reader->index += idx; 
        }
    }

    void Opc::ifne(Frame* frame) {
        uint32_t idx = frame->reader->u2B();
        int32_t value = frame->stack->pop32();
        if (value != 0) {
            frame->reader->index += idx; 
        }
    }

    void Opc::iflt(Frame* frame) {
        uint32_t idx = frame->reader->u2B();
        int32_t value = frame->stack->pop32();
        if (value < 0) {
            frame->reader->index += idx; 
        }
    }

    void Opc::ifge(Frame* frame) {
        uint32_t idx = frame->reader->u2B();
        int32_t value = frame->stack->pop32();
        if (value >= 0) {
            frame->reader->index += idx; 
        }
    }

    void Opc::ifgt(Frame* frame) {
        uint32_t idx = frame->reader->u2B();
        int32_t value = frame->stack->pop32();
        if (value > 0) {
            frame->reader->index += idx; 
        }
    }

    void Opc::ifle(Frame* frame) {
        uint32_t idx = frame->reader->u2B();
        int32_t value = frame->stack->pop32();
        if (value <= 0) {
            frame->reader->index += idx; 
        }
    }

    void Opc::ifnonnull(Frame* frame) {
        uint32_t idx = frame->reader->u2B();
        int32_t value = frame->stack->pop32();
        if (value != NULL) {
            frame->reader->index += idx; 
        }
    }

    void Opc::ifnull(Frame* frame) {
        uint32_t idx = frame->reader->u2B();
        int32_t value = frame->stack->pop32();
        if (value == NULL) {
            frame->reader->index += idx; 
        }
    }

    void Opc::iinc(Frame* frame) {

    }

    void Opc::iload(Frame* frame) {
        uint16_t idx = frame->widened ? frame->reader->u2B() : frame->reader->u1();
        frame->stack->push32(*(uint32_t*)((uint64_t) frame->locals + idx * 4));
        frame->widened = false;
    }

    void Opc::iload_0(Frame* frame) {
        frame->stack->push32(*(uint32_t*)((uint64_t) frame->locals));
    }

    void Opc::iload_1(Frame* frame) {
        frame->stack->push32(*(uint32_t*)((uint64_t) frame->locals + 1 * 4));
    }

    void Opc::iload_2(Frame* frame) {
        frame->stack->push32(*(uint32_t*)((uint64_t) frame->locals + 2 * 4));
    }

    void Opc::iload_3(Frame* frame) {
        frame->stack->push32(*(uint32_t*)((uint64_t) frame->locals + 3 * 4));
    }

    void Opc::imul(Frame* frame) {
        int32_t value2 = frame->stack->pop32();
        int32_t value1 = frame->stack->pop32();
        frame->stack->push32(value1 * value2);
    }

    void Opc::ineg(Frame* frame) {
        int32_t value = frame->stack->pop32();
        frame->stack->push32(-value);
    }

    void Opc::instanceof(Frame* frame) {

    }

    void Opc::invokedynamic(Frame* frame) {

    }

    void Opc::invokeinterface(Frame* frame) {

    }

    void Opc::invokespecial(Frame* frame) {

        uint32_t idx = frame->reader->u2B();

        Constant_RefInfo* mth = (Constant_RefInfo*) frame->constant_pool[idx];
        char* className = (char*) ((Constant_Utf8*) frame->constant_pool[((Constant_ClassInfo*) frame->constant_pool[mth->class_index])->name_index])->bytes;
        char* methodName = (char*) ((Constant_Utf8*) frame->constant_pool[((Constant_NameAndType*) frame->constant_pool[mth->name_and_type_index])->name_index])->bytes;
        char* signature = (char*) ((Constant_Utf8*) frame->constant_pool[((Constant_NameAndType*) frame->constant_pool[mth->name_and_type_index])->descriptor_index])->bytes;
        uint16_t signatureLength = ((Constant_Utf8*) frame->constant_pool[((Constant_NameAndType*) frame->constant_pool[mth->name_and_type_index])->descriptor_index])->length;

        ClassArea* cl = frame->jvm->getClasses()->getClass(className);

        uint16_t args_length = cl->getMethodArgsLength(methodName, signature); 
        uint32_t* args = NULL;
        if (args_length > 0) {
            args = (uint32_t*) malloc(args_length*4);
            frame->stack->sinkInto((uintptr_t) args, args_length*4);
        }
        uint32_t ret = cl->runMethod(frame->jvm, frame->stack->pop32(), methodName, signature, args, args_length);
        if (signature[signatureLength-1] != 'V') {
            // todo doesn't support long/double
            frame->stack->push32(ret);
        }

    }

    void Opc::invokestatic(Frame* frame) {

        uint32_t idx = frame->reader->u2B();

        Constant_RefInfo* mth = (Constant_RefInfo*) frame->constant_pool[idx];
        char* className = (char*) ((Constant_Utf8*) frame->constant_pool[((Constant_ClassInfo*) frame->constant_pool[mth->class_index])->name_index])->bytes;
        char* methodName = (char*) ((Constant_Utf8*) frame->constant_pool[((Constant_NameAndType*) frame->constant_pool[mth->name_and_type_index])->name_index])->bytes;
        char* signature = (char*) ((Constant_Utf8*) frame->constant_pool[((Constant_NameAndType*) frame->constant_pool[mth->name_and_type_index])->descriptor_index])->bytes;
        uint16_t signatureLength = ((Constant_Utf8*) frame->constant_pool[((Constant_NameAndType*) frame->constant_pool[mth->name_and_type_index])->descriptor_index])->length;

        ClassArea* cl = frame->jvm->getClasses()->getClass(className);

        uint16_t args_length = cl->getMethodArgsLength(methodName, signature); 
        uint32_t* args = NULL;
        if (args_length > 0) {
            args = (uint32_t*) malloc(args_length*4);
            frame->stack->sinkInto((uintptr_t) args, args_length*4);
        }
        uint32_t ret = cl->runStaticMethod(frame->jvm, methodName, signature, args, args_length);
        if (signature[signatureLength-1] != 'V') {
            // todo doesn't support long/double
            frame->stack->push32(ret);
        }

    }

    void Opc::invokevirtual(Frame* frame) {

        uint32_t idx = frame->reader->u2B();

        Constant_RefInfo* mth = (Constant_RefInfo*) frame->constant_pool[idx];
        char* className = (char*) ((Constant_Utf8*) frame->constant_pool[((Constant_ClassInfo*) frame->constant_pool[mth->class_index])->name_index])->bytes;
        char* methodName = (char*) ((Constant_Utf8*) frame->constant_pool[((Constant_NameAndType*) frame->constant_pool[mth->name_and_type_index])->name_index])->bytes;
        char* signature = (char*) ((Constant_Utf8*) frame->constant_pool[((Constant_NameAndType*) frame->constant_pool[mth->name_and_type_index])->descriptor_index])->bytes;
        uint16_t signatureLength = ((Constant_Utf8*) frame->constant_pool[((Constant_NameAndType*) frame->constant_pool[mth->name_and_type_index])->descriptor_index])->length;

        ClassArea* cl = frame->jvm->getClasses()->getClass(className);

        uint16_t args_length = cl->getMethodArgsLength(methodName, signature); 
        uint32_t* args = NULL;
        if (args_length > 0) {
            args = (uint32_t*) malloc(args_length*4);
            frame->stack->sinkInto((uintptr_t) args, args_length*4);
        }
        vector_t* rs = frame->jvm->getRefSys();
        uint32_t ret = cl->runMethod(frame->jvm, frame->stack->pop32(), methodName, signature, args, args_length);
        if (signature[signatureLength-1] != 'V') {
            // todo doesn't support long/double
            frame->stack->push32(ret);
        }

    }

    void Opc::ior(Frame* frame) {
        int32_t value2 = frame->stack->pop32();
        int32_t value1 = frame->stack->pop32();
        frame->stack->push32(value1 | value2);
    }

    void Opc::irem(Frame* frame) {
        int32_t value2 = frame->stack->pop32();
        int32_t value1 = frame->stack->pop32();
        frame->stack->push32(value1 % value2);
    }

    void Opc::ireturn(Frame* frame) {
        frame->returnValue = frame->stack->pop32();
        frame->code_length = 0;
    }

    void Opc::ishl(Frame* frame) {
        int32_t value2 = frame->stack->pop32();
        int32_t value1 = frame->stack->pop32();
        frame->stack->push32(value1 << value2);
    }

    void Opc::ishr(Frame* frame) {
        int32_t value2 = frame->stack->pop32();
        int32_t value1 = frame->stack->pop32();
        frame->stack->push32(value1 >> value2);
    }

    void Opc::istore(Frame* frame) {
        uint16_t idx = frame->widened ? frame->reader->u2B() : frame->reader->u1();
        *(uint32_t*)((uint64_t) frame->locals + idx * 4) = frame->stack->pop32();
        frame->widened = false;
    }

    void Opc::istore_0(Frame* frame) {
        *(uint32_t*)((uint64_t) frame->locals) = frame->stack->pop32();
    }

    void Opc::istore_1(Frame* frame) {
        *(uint32_t*)((uint64_t) frame->locals + 1 * 4) = frame->stack->pop32();
    }

    void Opc::istore_2(Frame* frame) {
        *(uint32_t*)((uint64_t) frame->locals + 2 * 4) = frame->stack->pop32();
    }

    void Opc::istore_3(Frame* frame) {
        *(uint32_t*)((uint64_t) frame->locals + 3 * 4) = frame->stack->pop32();
    }

    void Opc::isub(Frame* frame) {
        int32_t value2 = frame->stack->pop32();
        int32_t value1 = frame->stack->pop32();
        frame->stack->push32(value1 - value2);
    }

    void Opc::iushr(Frame* frame) {
        int32_t value2 = frame->stack->pop32();
        int32_t value1 = frame->stack->pop32();
        int32_t mask = 0x7fffffff;
        mask = mask >> value2;
        mask = (mask << 1) | 1;
        frame->stack->push32((value1 >> value2) & mask);
    }

    void Opc::ixor(Frame* frame) {
        int32_t value2 = frame->stack->pop32();
        int32_t value1 = frame->stack->pop32();
        frame->stack->push32(value1 ^ value2);
    }

    void Opc::jsr(Frame* frame) {
        uint16_t jmp = frame->reader->u2B();
        frame->stack->push32(frame->reader->index);
        frame->reader->index += jmp;
    }

    void Opc::jsr_w(Frame* frame) {
        uint32_t jmp = frame->reader->u4B();
        frame->stack->push32(frame->reader->index);
        frame->reader->index += jmp;
    }

    void Opc::l2d(Frame* frame) {

    }

    void Opc::l2f(Frame* frame) {

    }

    void Opc::l2i(Frame* frame) {
        int64_t val = frame->stack->pop64();
        frame->stack->push32(static_cast<int32_t>(val));
    }

    void Opc::ladd(Frame* frame) {
        int64_t value2 = frame->stack->pop64();
        int64_t value1 = frame->stack->pop64();
        frame->stack->push64(value1 + value2);
    }

    void Opc::laload(Frame* frame) {

    }

    void Opc::land(Frame* frame) {
        int64_t value2 = frame->stack->pop64();
        int64_t value1 = frame->stack->pop64();
        frame->stack->push64(value1 & value2);
    }

    void Opc::lastore(Frame* frame) {
        int64_t value = frame->stack->pop64();
        int32_t index = frame->stack->pop32();
        uint32_t arrayref = frame->stack->pop32();
        vector_t* rs = frame->jvm->getRefSys();
        uintptr_t arr = vector_get(rs, arrayref);
        *(uint64_t*)((uint64_t) arr + index * 8) = value;
    }

    void Opc::lcmp(Frame* frame) {
        int64_t value2 = frame->stack->pop64();
        int64_t value1 = frame->stack->pop64();
        if (value1 > value2) {
            frame->stack->push32(1);
        } else if (value1 == value2) {
            frame->stack->push32(0);
        } else {
            frame->stack->push32(-1);
        }
    }

    void Opc::lconst_0(Frame* frame) {
        frame->stack->push64((int64_t) 0);
    }

    void Opc::lconst_1(Frame* frame) {
        frame->stack->push64((int64_t) 1);
    }

    void Opc::ldc(Frame* frame) {
        uint8_t idx = frame->reader->u1();
        ConstantPoolEntry* entry = (ConstantPoolEntry*) frame->constant_pool[idx];
        vector_t* rs = frame->jvm->getRefSys();
        if (entry->tag == CONSTANT_String) {
            ClassArea* jlString = frame->jvm->getClasses()->getClass("java/lang/String");
            uintptr_t object = jlString->newObject();
            jlString->setField64(object, "<pointer>", (uint64_t) ((Constant_Utf8*) frame->constant_pool[((Constant_String*) entry)->string_index])->bytes);
            jlString->setField32(object, "<length>", (uint32_t) ((Constant_Utf8*) frame->constant_pool[((Constant_String*) entry)->string_index])->length);
            vector_push(rs, object);
            frame->stack->push32(rs->length-1);
        } else if (entry->tag == CONSTANT_Integer || entry->tag == CONSTANT_Float) {
            frame->stack->push32(((Constant_Integer_Float*) entry)->bytes);
        }
    }

    void Opc::ldc_w(Frame* frame) {
        uint16_t idx = frame->reader->u2B();
        ConstantPoolEntry* entry = (ConstantPoolEntry*) frame->constant_pool[idx];
        vector_t* rs = frame->jvm->getRefSys();
        if (entry->tag == CONSTANT_String) {
            ClassArea* jlString = frame->jvm->getClasses()->getClass("java/lang/String");
            uintptr_t object = jlString->newObject();
            jlString->setField64(object, "<pointer>", (uint64_t) ((Constant_Utf8*) frame->constant_pool[((Constant_String*) entry)->string_index])->bytes);
            jlString->setField32(object, "<length>", (uint32_t) ((Constant_Utf8*) frame->constant_pool[((Constant_String*) entry)->string_index])->length);
            vector_push(rs, object);
            frame->stack->push32(rs->length-1);
        } else if (entry->tag == CONSTANT_Integer || entry->tag == CONSTANT_Float) {
            frame->stack->push32(((Constant_Integer_Float*) entry)->bytes);
        }
    }

    void Opc::ldc2_w(Frame* frame) {
        uint16_t idx = frame->reader->u2B();
        ConstantPoolEntry* entry = (ConstantPoolEntry*) frame->constant_pool[idx];
        vector_t* rs = frame->jvm->getRefSys();
        // implement long and double
    }

    void Opc::ldiv(Frame* frame) {
        int64_t value2 = frame->stack->pop64();
        int64_t value1 = frame->stack->pop64();
        if (value2 == 0) {
            // throw arithmetic exception
        }
        frame->stack->push64(value1 / value2);
    }

    void Opc::lload(Frame* frame) {
        uint16_t idx = frame->widened ? frame->reader->u2B() : frame->reader->u1();
        frame->stack->push64(*(uint64_t*)((uint64_t) frame->locals + idx * 4));
        frame->widened = false;
    }

    void Opc::lload_0(Frame* frame) {
        frame->stack->push64(*(uint64_t*)((uint64_t) frame->locals));
    }

    void Opc::lload_1(Frame* frame) {
        frame->stack->push64(*(uint64_t*)((uint64_t) frame->locals + 1 * 4));
    }

    void Opc::lload_2(Frame* frame) {
        frame->stack->push64(*(uint64_t*)((uint64_t) frame->locals + 2 * 4));
    }

    void Opc::lload_3(Frame* frame) {
        frame->stack->push64(*(uint64_t*)((uint64_t) frame->locals + 3 * 4));
    }

    void Opc::lmul(Frame* frame) {
        int64_t value2 = frame->stack->pop64();
        int64_t value1 = frame->stack->pop64();
        frame->stack->push64(value1 * value2);
    }

    void Opc::lneg(Frame* frame) {
        int64_t value = frame->stack->pop64();
        frame->stack->push64(-value);
    }

    void Opc::lookupswitch(Frame* frame) {

    }

    void Opc::lor(Frame* frame) {
        int64_t value2 = frame->stack->pop64();
        int64_t value1 = frame->stack->pop64();
        frame->stack->push64(value1 | value2);
    }

    void Opc::lrem(Frame* frame) {
        int64_t value2 = frame->stack->pop64();
        int64_t value1 = frame->stack->pop64();
        frame->stack->push64(value1 % value2);
    }

    void Opc::lreturn(Frame* frame) {
        frame->returnValue = frame->stack->pop64();
        frame->code_length = 0;
    }

    void Opc::lshl(Frame* frame) {
        int64_t value2 = frame->stack->pop64();
        int32_t value1 = frame->stack->pop32();
        frame->stack->push64(value1 << value2);
    }

    void Opc::lshr(Frame* frame) {
        int64_t value2 = frame->stack->pop64();
        int32_t value1 = frame->stack->pop32();
        frame->stack->push64(value1 >> value2);
    }

    void Opc::lstore(Frame* frame) {
        uint16_t idx = frame->widened ? frame->reader->u2B() : frame->reader->u1();
        *(uint64_t*)((uint64_t) frame->locals + idx * 4) = frame->stack->pop64();
        frame->widened = false;
    }

    void Opc::lstore_0(Frame* frame) {
        *(uint64_t*)((uint64_t) frame->locals) = frame->stack->pop64();
    }

    void Opc::lstore_1(Frame* frame) {
        *(uint64_t*)((uint64_t) frame->locals + 1 * 4) = frame->stack->pop64();
    }

    void Opc::lstore_2(Frame* frame) {
        *(uint64_t*)((uint64_t) frame->locals + 2 * 4) = frame->stack->pop64();
    }

    void Opc::lstore_3(Frame* frame) {
        *(uint64_t*)((uint64_t) frame->locals + 3 * 4) = frame->stack->pop64();
    }

    void Opc::lsub(Frame* frame) {
        int64_t value2 = frame->stack->pop64();
        int64_t value1 = frame->stack->pop64();
        frame->stack->push64(value1 - value2);
    }

    void Opc::lushr(Frame* frame) {
        int32_t value2 = frame->stack->pop32();
        int64_t value1 = frame->stack->pop64();
        int64_t mask = 0x7fffffffffffffff;
        mask = mask >> value2;
        mask = (mask << 1) | 1;
        frame->stack->push64((value1 >> value2) & mask);
    }

    void Opc::lxor(Frame* frame) {
        int64_t value2 = frame->stack->pop64();
        int64_t value1 = frame->stack->pop64();
        frame->stack->push64(value1 | value2);
    }

    void Opc::monitorenter(Frame* frame) {

    }

    void Opc::monitorexit(Frame* frame) {

    }

    void Opc::multianewarray(Frame* frame) {

    }

    void Opc::new_(Frame* frame) {
        uint16_t idx = frame->reader->u2B();
        Constant_ClassInfo* classInfo = (Constant_ClassInfo*) frame->constant_pool[idx];
        char* className = (char*) ((Constant_Utf8*) frame->constant_pool[classInfo->name_index])->bytes;
        ClassArea* cl = frame->jvm->getClasses()->getClass(className);
        vector_t* rs = frame->jvm->getRefSys();
        uintptr_t obj = cl->newObject();
        vector_push(rs, obj);
        frame->stack->push32(rs->length-1);
    }

    uint8_t getArrayTypeSize(uint8_t type) {
        switch (type) {
            case SE8::AT_BOOLEAN:
            case SE8::AT_BYTE:
                return 1;
            case SE8::AT_CHAR:
            case SE8::AT_SHORT:
                return 2;
            case SE8::AT_INT:
            case SE8::AT_FLOAT:
                return 4;
            case SE8::AT_LONG:
            case SE8::AT_DOUBLE:
                return 8;
        }
    }

    void Opc::newarray(Frame* frame) {
        uint8_t itemSize = getArrayTypeSize(frame->reader->u1());
        int32_t count = frame->stack->pop32();
        if (count < 0) {
            // throw NegativeArraySizeException
        } else {
            // todo: should use garbage collection to allocate array
            uintptr_t arr = malloc(itemSize*count);
            vector_t* rs = frame->jvm->getRefSys();
            vector_push(rs, arr);
            frame->stack->push32(rs->length-1);
        }
    }

    void Opc::nop(Frame* frame) {
        // do nothing
    }

    void Opc::pop(Frame* frame) {
        frame->stack->pop32();
    }

    void Opc::pop2(Frame* frame) {
        frame->stack->pop64();
    }

    void Opc::putfield(Frame* frame) {

    }

    void Opc::putstatic(Frame* frame) {
        uint16_t idx = frame->reader->u2B();
        Constant_RefInfo* field = (Constant_RefInfo*) frame->constant_pool[idx];
        char* className = (char*) ((Constant_Utf8*) frame->constant_pool[((Constant_ClassInfo*) frame->constant_pool[field->class_index])->name_index])->bytes;
        char* fieldName = (char*) ((Constant_Utf8*) frame->constant_pool[((Constant_NameAndType*) frame->constant_pool[field->name_and_type_index])->name_index])->bytes;
        ClassArea* cl = frame->jvm->getClasses()->getClass(className);
        uint8_t fieldSize = cl->getStaticFieldSize(fieldName);
        switch (fieldSize) {
            case 1:
                cl->setStaticField8(fieldName, frame->stack->pop8());
                break;
            case 2:
                cl->setStaticField16(fieldName, frame->stack->pop16());
                break;
            case 4:
                cl->setStaticField32(fieldName, frame->stack->pop32());
                break;
            case 8:
                cl->setStaticField64(fieldName, frame->stack->pop64());
                break;
        }
    }

    void Opc::ret(Frame* frame) {
        uint16_t idx = frame->widened ? frame->reader->u2B() : frame->reader->u1();
        frame->reader->index = *(uint32_t*)((uint64_t) frame->locals + idx * 4);
        frame->widened = false;
    }

    void Opc::return_(Frame* frame) {
        frame->returnValue = NULL;
        frame->code_length = 0;
    }

    void Opc::saload(Frame* frame) {

    }

    void Opc::sastore(Frame* frame) {
        uint32_t value = frame->stack->pop32();
        int32_t index = frame->stack->pop32();
        uint32_t arrayref = frame->stack->pop32();
        vector_t* rs = frame->jvm->getRefSys();
        uintptr_t arr = vector_get(rs, arrayref);
        *(uint16_t*)((uint64_t) arr + index * 2) = (uint16_t) value;
    }

    void Opc::sipush(Frame* frame) {
        frame->stack->push32(frame->reader->u2B());
    }

    void Opc::swap(Frame* frame) {
        int32_t value1 = frame->stack->pop32();
        int32_t value2 = frame->stack->pop32();
        frame->stack->push32(value1);
        frame->stack->push32(value2);
    }

    void Opc::tableswitch(Frame* frame) {

    }

    void Opc::wide(Frame* frame) {
        frame->widened = true;
    }

    OpCodeTable* oct = NULL;

    bool isOpCodeTableNotInit() {
        return (oct == NULL);
    }

    void initOpCodeTable() {
        oct = (OpCodeTable*) malloc(sizeof(OpCodeTable)*400);
        oct[SE8::aaload] = Opc::aaload;
        oct[SE8::aastore] = Opc::aastore;
        oct[SE8::aconst_null] = Opc::aconst_null;
        oct[SE8::aload] = Opc::aload;
        oct[SE8::aload_0] = Opc::aload_0;
        oct[SE8::aload_1] = Opc::aload_1;
        oct[SE8::aload_2] = Opc::aload_2;
        oct[SE8::aload_3] = Opc::aload_3;
        oct[SE8::anewarray] = Opc::anewarray;
        oct[SE8::areturn] = Opc::areturn;
        oct[SE8::arraylength] = Opc::arraylength;
        oct[SE8::astore] = Opc::astore;
        oct[SE8::astore_0] = Opc::astore_0;
        oct[SE8::astore_1] = Opc::astore_1;
        oct[SE8::astore_2] = Opc::astore_2;
        oct[SE8::astore_3] = Opc::astore_3;
        oct[SE8::athrow] = Opc::athrow;
        oct[SE8::baload] = Opc::baload;
        oct[SE8::bastore] = Opc::bastore;
        oct[SE8::bipush] = Opc::bipush;
        oct[SE8::caload] = Opc::caload;
        oct[SE8::castore] = Opc::castore;
        oct[SE8::checkcast] = Opc::checkcast;
        oct[SE8::d2f] = Opc::d2f;
        oct[SE8::d2i] = Opc::d2i;
        oct[SE8::d2l] = Opc::d2l;
        oct[SE8::dadd] = Opc::dadd;
        oct[SE8::daload] = Opc::daload;
        oct[SE8::dastore] = Opc::dastore;
        oct[SE8::dcmpg] = Opc::dcmpg;
        oct[SE8::dcmpl] = Opc::dcmpl;
        oct[SE8::dconst_0] = Opc::dconst_0;
        oct[SE8::dconst_1] = Opc::dconst_1;
        oct[SE8::ddiv] = Opc::ddiv;
        oct[SE8::dload] = Opc::dload;
        oct[SE8::dload_0] = Opc::dload_0;
        oct[SE8::dload_1] = Opc::dload_1;
        oct[SE8::dload_2] = Opc::dload_2;
        oct[SE8::dload_3] = Opc::dload_3;
        oct[SE8::dmul] = Opc::dmul;
        oct[SE8::dneg] = Opc::dneg;
        oct[SE8::drem] = Opc::drem;
        oct[SE8::dreturn] = Opc::dreturn;
        oct[SE8::dstore] = Opc::dstore;
        oct[SE8::dstore_0] = Opc::dstore_0;
        oct[SE8::dstore_1] = Opc::dstore_1; 
        oct[SE8::dstore_2] = Opc::dstore_2;
        oct[SE8::dstore_3] = Opc::dstore_3;
        oct[SE8::dsub] = Opc::dsub;
        oct[SE8::dup] = Opc::dup;
        oct[SE8::dup_x1] = Opc::dup_x1;
        oct[SE8::dup_x2] = Opc::dup_x2;
        oct[SE8::dup2] = Opc::dup2;
        oct[SE8::dup2_x1] = Opc::dup2_x1;
        oct[SE8::dup2_x2] = Opc::dup2_x2;
        oct[SE8::f2d] = Opc::f2d;
        oct[SE8::f2i] = Opc::f2i;
        oct[SE8::f2l] = Opc::f2l;
        oct[SE8::fadd] = Opc::fadd;
        oct[SE8::faload] = Opc::faload;
        oct[SE8::fastore] = Opc::fastore;
        oct[SE8::fcmpg] = Opc::fcmpg;
        oct[SE8::fcpml] = Opc::fcpml;
        oct[SE8::fconst_0] = Opc::fconst_0;
        oct[SE8::fconst_1] = Opc::fconst_1;
        oct[SE8::fconst_2] = Opc::fconst_2;
        oct[SE8::fdiv] = Opc::fdiv;
        oct[SE8::fload] = Opc::fload;
        oct[SE8::fload_0] = Opc::fload_0;
        oct[SE8::fload_1] = Opc::fload_1;
        oct[SE8::fload_2] = Opc::fload_2;
        oct[SE8::fload_3] = Opc::fload_3;
        oct[SE8::fmul] = Opc::fmul;
        oct[SE8::fneg] = Opc::fneg;
        oct[SE8::frem] = Opc::frem;
        oct[SE8::freturn] = Opc::freturn;
        oct[SE8::fstore] = Opc::fstore;
        oct[SE8::fstore_0] = Opc::fstore_0;
        oct[SE8::fstore_1] = Opc::fstore_1;
        oct[SE8::fstore_2] = Opc::fstore_2;
        oct[SE8::fstore_3] = Opc::fstore_3;
        oct[SE8::fsub] = Opc::fsub;
        oct[SE8::getfield] = Opc::getfield;
        oct[SE8::getstatic] = Opc::getstatic;
        oct[SE8::goto_] = Opc::goto_;
        oct[SE8::goto_w] = Opc::goto_w;
        oct[SE8::i2b] = Opc::i2b;
        oct[SE8::i2c] = Opc::i2c;
        oct[SE8::i2d] = Opc::i2d;
        oct[SE8::i2f] = Opc::i2f;
        oct[SE8::i2l] = Opc::i2l;
        oct[SE8::i2s] = Opc::i2s;
        oct[SE8::iadd] = Opc::iadd;
        oct[SE8::iaload] = Opc::iaload;
        oct[SE8::iand] = Opc::iand;
        oct[SE8::iastore] = Opc::iastore;
        oct[SE8::iconst_m1] = Opc::iconst_m1;
        oct[SE8::iconst_0] = Opc::iconst_0;
        oct[SE8::iconst_1] = Opc::iconst_1;
        oct[SE8::iconst_2] = Opc::iconst_2;
        oct[SE8::iconst_3] = Opc::iconst_3;
        oct[SE8::iconst_4] = Opc::iconst_4;
        oct[SE8::iconst_5] = Opc::iconst_5;
        oct[SE8::idiv] = Opc::idiv;
        oct[SE8::if_acmpeq] = Opc::if_acmpeq;
        oct[SE8::if_acmpne] = Opc::if_acmpne;
        oct[SE8::if_icmpeq] = Opc::if_icmpeq;
        oct[SE8::if_icmpne] = Opc::if_icmpne;
        oct[SE8::if_icmplt] = Opc::if_icmplt;
        oct[SE8::if_icmpge] = Opc::if_icmpge;
        oct[SE8::if_icmpgt] = Opc::if_icmpgt;
        oct[SE8::if_icmple] = Opc::if_icmple;
        oct[SE8::ifeq] = Opc::ifeq;
        oct[SE8::ifne] = Opc::ifne;
        oct[SE8::iflt] = Opc::iflt;
        oct[SE8::ifge] = Opc::ifge;
        oct[SE8::ifgt] = Opc::ifgt;
        oct[SE8::ifle] = Opc::ifle;
        oct[SE8::ifnonnull] = Opc::ifnonnull;
        oct[SE8::ifnull] = Opc::ifnull;
        oct[SE8::iinc] = Opc::iinc;
        oct[SE8::iload] = Opc::iload;
        oct[SE8::iload_0] = Opc::iload_0;
        oct[SE8::iload_1] = Opc::iload_1;
        oct[SE8::iload_2] = Opc::iload_2;
        oct[SE8::iload_3] = Opc::iload_3;
        oct[SE8::imul] = Opc::imul;
        oct[SE8::ineg] = Opc::ineg;
        oct[SE8::instanceof] = Opc::instanceof;
        oct[SE8::invokedynamic] = Opc::invokedynamic;
        oct[SE8::invokeinterface] = Opc::invokeinterface;
        oct[SE8::invokespecial] = Opc::invokespecial;
        oct[SE8::invokestatic] = Opc::invokestatic;
        oct[SE8::invokevirtual] = Opc::invokevirtual;
        oct[SE8::ior] = Opc::ior;
        oct[SE8::irem] = Opc::irem;
        oct[SE8::ireturn] = Opc::ireturn;
        oct[SE8::ishl] = Opc::ishl;
        oct[SE8::ishr] = Opc::ishr;
        oct[SE8::istore] = Opc::istore;
        oct[SE8::istore_0] = Opc::istore_0;
        oct[SE8::istore_1] = Opc::istore_1;
        oct[SE8::istore_2] = Opc::istore_2;
        oct[SE8::istore_3] = Opc::istore_3;
        oct[SE8::isub] = Opc::isub;
        oct[SE8::iushr] = Opc::iushr;
        oct[SE8::ixor] = Opc::ixor;
        oct[SE8::jsr] = Opc::jsr;
        oct[SE8::jsr_w] = Opc::jsr_w;
        oct[SE8::l2d] = Opc::l2d;
        oct[SE8::l2f] = Opc::l2f;
        oct[SE8::l2i] = Opc::l2i;
        oct[SE8::ladd] = Opc::ladd;
        oct[SE8::laload] = Opc::laload;
        oct[SE8::land] = Opc::land;
        oct[SE8::lastore] = Opc::lastore;
        oct[SE8::lcmp] = Opc::lcmp;
        oct[SE8::lconst_0] = Opc::lconst_0;
        oct[SE8::lconst_1] = Opc::lconst_1;
        oct[SE8::ldc] = Opc::ldc;
        oct[SE8::ldc_w] = Opc::ldc_w;
        oct[SE8::ldc2_w] = Opc::ldc2_w;
        oct[SE8::ldiv] = Opc::ldiv;
        oct[SE8::lload] = Opc::lload;
        oct[SE8::lload_0] = Opc::lload_0;
        oct[SE8::lload_1] = Opc::lload_1;
        oct[SE8::lload_2] = Opc::lload_2;
        oct[SE8::lload_3] = Opc::lload_3;
        oct[SE8::lmul] = Opc::lmul;
        oct[SE8::lneg] = Opc::lneg;
        oct[SE8::lookupswitch] = Opc::lookupswitch;
        oct[SE8::lor] = Opc::lor;
        oct[SE8::lrem] = Opc::lrem;
        oct[SE8::lreturn] = Opc::lreturn;
        oct[SE8::lshl] = Opc::lshl;
        oct[SE8::lshr] = Opc::lshr;
        oct[SE8::lstore] = Opc::lstore;
        oct[SE8::lstore_0] = Opc::lstore_0;
        oct[SE8::lstore_1] = Opc::lstore_1;
        oct[SE8::lstore_2] = Opc::lstore_2;
        oct[SE8::lstore_3] = Opc::lstore_3;
        oct[SE8::lsub] = Opc::lsub;
        oct[SE8::lushr] = Opc::lushr;
        oct[SE8::lxor] = Opc::lxor;
        oct[SE8::monitorenter] = Opc::monitorenter;
        oct[SE8::monitorexit] = Opc::monitorexit;
        oct[SE8::multianewarray] = Opc::multianewarray;
        oct[SE8::new_] = Opc::new_;
        oct[SE8::newarray] = Opc::newarray;
        oct[SE8::nop] = Opc::nop;
        oct[SE8::pop] = Opc::pop;
        oct[SE8::pop2] = Opc::pop2;
        oct[SE8::putfield] = Opc::putfield;
        oct[SE8::putstatic] = Opc::putstatic;
        oct[SE8::ret] = Opc::ret;
        oct[SE8::return_] = Opc::return_;
        oct[SE8::saload] = Opc::saload;
        oct[SE8::sastore] = Opc::sastore;
        oct[SE8::sipush] = Opc::sipush;
        oct[SE8::swap] = Opc::swap;
        oct[SE8::tableswitch] = Opc::tableswitch;
        oct[SE8::wide] = Opc::wide;
    }

    void Frame::run(uintptr_t args, uint32_t args_length) {
        widened = false;
        reader->index = 0;
        if (args_length > 0) {
            for (uint16_t i = 0; i < args_length; i++) {
                // todo
            }
        }
        while (reader->index < code_length) {
            uint8_t oc = reader->u1();
            //Printlog(itoa(oc, "    ", 16));
            oct[oc](this);
        }
        free(stack);
        free(locals);
    }

}
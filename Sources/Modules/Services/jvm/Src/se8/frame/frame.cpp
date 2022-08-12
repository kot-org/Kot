#include "frame.h"

uint8_t getComputationalCategory(uint8_t type) {
    switch (type) {
        case SE8::Long:
        case SE8::Double:
            return 2;
        default:
            return 1;
    }
}

namespace SE8 {

    Frame::Frame(JVM* jvm, uintptr_t constant_pool) {
        this->constant_pool = constant_pool;
        this->stack = new Stack(jvm->getStackSize());
        this->reader = (Reader*) malloc(sizeof(Reader));
        this->locals = new Locals();
    }

    void Frame::aaload() {

    }

    void Frame::aastore() {

    }

    void Frame::aconst_null() {
        stack->pushNull();
    }

    void Frame::aload() {

    }

    void Frame::aload_0() {

    }

    void Frame::aload_1() {

    }

    void Frame::aload_2() {

    }

    void Frame::aload_3() {

    }

    void Frame::anewarray(){ 

    }

    void Frame::areturn() {

    }

    void Frame::arraylength() {

    }

    void Frame::astore() {

    }

    void Frame::astore_0() {

    }

    void Frame::astore_1() {

    }

    void Frame::astore_2() {

    }

    void Frame::astore_3() {

    }

    void Frame::athrow() {

    }

    void Frame::baload() {

    }

    void Frame::bastore() {

    }

    void Frame::bipush() {
        stack->pushByte(reader->u1());
    }

    void Frame::caload() {

    }

    void Frame::castore() {

    }

    void Frame::checkcast() {

    }

    void Frame::d2f() {
        SE8::Value* value = stack->pop();
        if (value->type == SE8::Double) {
            double val = *(uint64_t*)((uint64_t) value + 1);
            stack->pushFloat(static_cast<float>(val));
        } else {
            // throw type exception
        }
    }

    void Frame::d2i() {
        SE8::Value* value = stack->pop();
        if (value->type == SE8::Double) {
            double val = *(uint64_t*)((uint64_t) value + 1);
            stack->pushInt(static_cast<int32_t>(val));
        } else {
            // throw type exception
        }
    }

    void Frame::d2l() {
        SE8::Value* value = stack->pop();
        if (value->type == SE8::Double) {
            double val = *(uint64_t*)((uint64_t) value + 1);
            stack->pushLong(static_cast<int64_t>(val));
        } else {
            // throw type exception
        }
    }

    void Frame::dadd() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::Double && value2->type == SE8::Double) {
            double val1 = *(uint64_t*)((uint64_t) value1 + 1);
            double val2 = *(uint64_t*)((uint64_t) value2 + 1);
            stack->pushDouble(val1 + val2);
        } else {
            // throw type exception
        }
    }

    void Frame::daload() {

    }

    void Frame::dastore() {
        
    }

    void Frame::dcmpg() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::NaN || value2->type == SE8::NaN) {
            stack->pushInt(1);
        } else if (value1->type == SE8::Double && value2->type == SE8::Double) {
            double val1 = *(uint64_t*)((uint64_t) value1 + 1);
            double val2 = *(uint64_t*)((uint64_t) value2 + 1);
            if (val1 == val2) {
                stack->pushInt(0);
            } else if (val1 > val2) {
                stack->pushInt(1);
            } else if (val1 < val2) {
                stack->pushInt(-1);
            }
        } else {
            // throw type exception
        }
    }

    void Frame::dcmpl() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::NaN || value2->type == SE8::NaN) {
            stack->pushInt(-1);
        } else if (value1->type == SE8::Double && value2->type == SE8::Double) {
            double val1 = *(uint64_t*)((uint64_t) value1 + 1);
            double val2 = *(uint64_t*)((uint64_t) value2 + 1);
            if (val1 == val2) {
                stack->pushInt(0);
            } else if (val1 > val2) {
                stack->pushInt(1);
            } else if (val1 < val2) {
                stack->pushInt(-1);
            }
        } else {
            // throw type exception
        }
    }

    void Frame::dconst_0() {
        stack->pushDouble(0.0);
    }

    void Frame::dconst_1() {
        stack->pushDouble(1.0);
    }

    void Frame::ddiv() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::Double && value2->type == SE8::Double) {
            double val2 = *(uint64_t*)((uint64_t) value2 + 1);
            if (val2 == 0) {
                stack->pushNaN();
            } else {
                double val1 = *(uint64_t*)((uint64_t) value1 + 1);
                stack->pushDouble(val1 / val2);
            }
        } else {
            // throw type exception
        }
    }

    void Frame::dload() {
        uint64_t ptr = locals->getPtr(widened ? reader->u2L() : reader->u1());
        uint8_t type = locals->getType(ptr);
        if (type == SE8::Double) {
            stack->pushDouble(locals->get64(ptr));
        } else if (type == SE8::NaN) {
            stack->pushNaN();
        } else if (type == SE8::Null) {
            stack->pushNull();
        } else {
            // throw type exception
        }
        widened = false;
    }

    void Frame::dload_0() {
        uint64_t ptr = locals->getPtr(0);
        uint8_t type = locals->getType(ptr);
        if (type == SE8::Double) {
            stack->pushDouble(locals->get64(ptr));
        } else if (type == SE8::NaN) {
            stack->pushNaN();
        } else if (type == SE8::Null) {
            stack->pushNull();
        } else {
            // throw type exception
        }
        widened = false;
    }

    void Frame::dload_1() {
        uint64_t ptr = locals->getPtr(1);
        uint8_t type = locals->getType(ptr);
        if (type == SE8::Double) {
            stack->pushDouble(locals->get64(ptr));
        } else if (type == SE8::NaN) {
            stack->pushNaN();
        } else if (type == SE8::Null) {
            stack->pushNull();
        } else {
            // throw type exception
        }
        widened = false;
    }

    void Frame::dload_2() {
        uint64_t ptr = locals->getPtr(2);
        uint8_t type = locals->getType(ptr);
        if (type == SE8::Double) {
            stack->pushDouble(locals->get64(ptr));
        } else if (type == SE8::NaN) {
            stack->pushNaN();
        } else if (type == SE8::Null) {
            stack->pushNull();
        } else {
            // throw type exception
        }
        widened = false;
    }

    void Frame::dload_3() {
        uint64_t ptr = locals->getPtr(3);
        uint8_t type = locals->getType(ptr);
        if (type == SE8::Double) {
            stack->pushDouble(locals->get64(ptr));
        } else if (type == SE8::NaN) {
            stack->pushNaN();
        } else if (type == SE8::Null) {
            stack->pushNull();
        } else {
            // throw type exception
        }
        widened = false;
    }

    void Frame::dmul() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::Double && value2->type == SE8::Double) {
            double val1 = *(uint64_t*)((uint64_t) value1 + 1);
            double val2 = *(uint64_t*)((uint64_t) value2 + 1);
            stack->pushDouble(val1 * val2);
        } else {
            // throw type exception
        }
    }

    void Frame::dneg() {
        SE8::Value* value = stack->pop();
        if (value->type == SE8::Double) {
            double val = *(uint64_t*)((uint64_t) value + 1);
            stack->pushDouble(-val);
        } else {
            // throw type exception
        }
    }

    void Frame::drem() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::Double && value2->type == SE8::Double) {
            double val1 = *(uint64_t*)((uint64_t) value1 + 1);
            double val2 = *(uint64_t*)((uint64_t) value2 + 1);
            stack->pushDouble(val1 * val2);
        } else {
            // throw type exception
        }
    }

    void Frame::dreturn() {

    }

    void Frame::dstore() {
        uint64_t ptr = locals->getPtr(widened ? reader->u2L() : reader->u1());
        SE8::Value* value = stack->pop();
        locals->setType(ptr, value->type);
        if (value->type == SE8::Double) {
            locals->set64(ptr, *(uint64_t*)(&value->bytes));
        }
        widened = false;
    }

    void Frame::dstore_0() {
        uint64_t ptr = locals->getPtr(0);
        SE8::Value* value = stack->pop();
        locals->setType(ptr, value->type);
        if (value->type == SE8::Double) {
            locals->set64(ptr, *(uint64_t*)(&value->bytes));
        }
        widened = false;
    }

    void Frame::dstore_1() {
        uint64_t ptr = locals->getPtr(1);
        SE8::Value* value = stack->pop();
        locals->setType(ptr, value->type);
        if (value->type == SE8::Double) {
            locals->set64(ptr, *(uint64_t*)(&value->bytes));
        }
        widened = false;
    }

    void Frame::dstore_2() {
        uint64_t ptr = locals->getPtr(2);
        SE8::Value* value = stack->pop();
        locals->setType(ptr, value->type);
        if (value->type == SE8::Double) {
            locals->set64(ptr, *(uint64_t*)(&value->bytes));
        }
        widened = false;
    }

    void Frame::dstore_3() {
        uint64_t ptr = locals->getPtr(3);
        SE8::Value* value = stack->pop();
        locals->setType(ptr, value->type);
        if (value->type == SE8::Double) {
            locals->set64(ptr, *(uint64_t*)(&value->bytes));
        }
        widened = false;
    }

    void Frame::dsub() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::Double && value2->type == SE8::Double) {
            double val1 = *(uint64_t*)((uint64_t) value1 + 1);
            double val2 = *(uint64_t*)((uint64_t) value2 + 1);
            stack->pushDouble(val1 - val2);
        } else {
            // throw type exception
        }
    }

    void Frame::dup() {
        stack->push(stack->peek());
    }

    void Frame::dup_x1() {
        SE8::Value* val1 = stack->pop();
        SE8::Value* val2 = stack->pop();
        stack->push(val1);
        stack->push(val2);
        stack->push(val1);
    }

    void Frame::dup_x2() {
        SE8::Value* val1 = stack->pop();
        SE8::Value* val2 = stack->pop();
        SE8::Value* val3 = stack->pop();
        stack->push(val1);
        stack->push(val3);
        stack->push(val2);
        stack->push(val1);
    }

    void Frame::dup2() {
        SE8::Value* val1 = stack->pop();
        SE8::Value* val2 = stack->pop();
        stack->push(val2);
        stack->push(val1);
        stack->push(val2);
        stack->push(val1);
    }

    void Frame::dup2_x1() {
        SE8::Value* val1 = stack->pop();
        SE8::Value* val2 = stack->pop();
        SE8::Value* val3 = stack->pop();
        stack->push(val2);
        stack->push(val1);
        stack->push(val3);
        stack->push(val2);
        stack->push(val1);
    }

    void Frame::dup2_x2() {
        SE8::Value* val1 = stack->pop();
        SE8::Value* val2 = stack->pop();
        SE8::Value* val3 = stack->pop();
        SE8::Value* val4 = stack->pop();
        stack->push(val2);
        stack->push(val1);
        stack->push(val4);
        stack->push(val3);
        stack->push(val2);
        stack->push(val1);
    }

    void Frame::f2d() {
        SE8::Value* value = stack->pop();
        if (value->type == SE8::Float) {
            float val = *(uint32_t*)((uint64_t) value + 1);
            stack->pushDouble(static_cast<double>(val));
        } else {
            // throw type exception
        }
    }

    void Frame::f2i() {
        SE8::Value* value = stack->pop();
        if (value->type == SE8::Float) {
            float val = *(uint32_t*)((uint64_t) value + 1);
            stack->pushInt(static_cast<int32_t>(val));
        } else {
            // throw type exception
        }
    }

    void Frame::f2l() {
        SE8::Value* value = stack->pop();
        if (value->type == SE8::Float) {
            float val = *(uint32_t*)((uint64_t) value + 1);
            stack->pushLong(static_cast<int64_t>(val));
        } else {
            // throw type exception
        }
    }

    void Frame::fadd() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::Float && value2->type == SE8::Float) {
            float val1 = *(uint32_t*)((uint64_t) value1 + 1);
            float val2 = *(uint32_t*)((uint64_t) value2 + 1);
            stack->pushFloat(val1 + val2);
        } else {
            // throw type exception
        }
    }

    void Frame::faload() {

    }

    void Frame::fastore() {

    }

    void Frame::fcmpg() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::NaN || value2->type == SE8::NaN) {
            stack->pushInt(1);
        } else if (value1->type == SE8::Float && value2->type == SE8::Float) {
            float val1 = *(uint32_t*)((uint64_t) value1 + 1);
            float val2 = *(uint32_t*)((uint64_t) value2 + 1);
            if (val1 == val2) {
                stack->pushInt(0);
            } else if (val1 > val2) {
                stack->pushInt(1);
            } else if (val1 < val2) {
                stack->pushInt(-1);
            }
        } else {
            // throw type exception
        }
    }

    void Frame::fcpml() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::NaN || value2->type == SE8::NaN) {
            stack->pushInt(-1);
        } else if (value1->type == SE8::Float && value2->type == SE8::Float) {
            float val1 = *(uint32_t*)((uint64_t) value1 + 1);
            float val2 = *(uint32_t*)((uint64_t) value2 + 1);
            if (val1 == val2) {
                stack->pushInt(0);
            } else if (val1 > val2) {
                stack->pushInt(1);
            } else if (val1 < val2) {
                stack->pushInt(-1);
            }
        } else {
            // throw type exception
        }
    }

    void Frame::fconst_0() {
        stack->pushFloat(0.0);
    }

    void Frame::fconst_1() {
        stack->pushFloat(1.0);
    }

    void Frame::fconst_2() {
        stack->pushFloat(2.0);
    }

    void Frame::fdiv() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::Float && value2->type == SE8::Float) {
            float val2 = *(uint32_t*)((uint64_t) value2 + 1);
            if (val2 == 0) {
                stack->pushNaN();
            } else {
                float val1 = *(uint32_t*)((uint64_t) value1 + 1);
                stack->pushFloat(val1 / val2);
            }
        } else {
            // throw type exception
        }
    }

    void Frame::fload() {
        uint64_t ptr = locals->getPtr(widened ? reader->u2L() : reader->u1());
        uint8_t type = locals->getType(ptr);
        if (type == SE8::Float) {
            stack->pushFloat(locals->get32(ptr));
        } else if (type == SE8::NaN) {
            stack->pushNaN();
        } else if (type == SE8::Null) {
            stack->pushNull();
        } else {
            // throw type exception
        }
        widened = false;
    }

    void Frame::fload_0() {
        uint64_t ptr = locals->getPtr(0);
        uint8_t type = locals->getType(ptr);
        if (type == SE8::Float) {
            stack->pushFloat(locals->get32(ptr));
        } else if (type == SE8::NaN) {
            stack->pushNaN();
        } else if (type == SE8::Null) {
            stack->pushNull();
        } else {
            // throw type exception
        }
        widened = false;
    }

    void Frame::fload_1() {
        uint64_t ptr = locals->getPtr(1);
        uint8_t type = locals->getType(ptr);
        if (type == SE8::Float) {
            stack->pushFloat(locals->get32(ptr));
        } else if (type == SE8::NaN) {
            stack->pushNaN();
        } else if (type == SE8::Null) {
            stack->pushNull();
        } else {
            // throw type exception
        }
        widened = false;
    }

    void Frame::fload_2() {
        uint64_t ptr = locals->getPtr(2);
        uint8_t type = locals->getType(ptr);
        if (type == SE8::Float) {
            stack->pushFloat(locals->get32(ptr));
        } else if (type == SE8::NaN) {
            stack->pushNaN();
        } else if (type == SE8::Null) {
            stack->pushNull();
        } else {
            // throw type exception
        }
        widened = false;
    }

    void Frame::fload_3() {
        uint64_t ptr = locals->getPtr(3);
        uint8_t type = locals->getType(ptr);
        if (type == SE8::Float) {
            stack->pushFloat(locals->get32(ptr));
        } else if (type == SE8::NaN) {
            stack->pushNaN();
        } else if (type == SE8::Null) {
            stack->pushNull();
        } else {
            // throw type exception
        }
        widened = false;
    }

    void Frame::fmul() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::Float && value2->type == SE8::Float) {
            float val1 = *(uint32_t*)((uint64_t) value1 + 1);
            float val2 = *(uint32_t*)((uint64_t) value2 + 1);
            stack->pushFloat(val1 * val2);
        } else {
            // throw type exception
        }
    }

    void Frame::fneg() {
        SE8::Value* value = stack->pop();
        if (value->type == SE8::Float) {
            float val = *(uint32_t*)((uint64_t) value + 1);
            stack->pushFloat(-val);
        } else {
            // throw type exception
        }
    }

    void Frame::frem() {

    }

    void Frame::freturn() {

    }

    void Frame::fstore() {
        uint64_t ptr = locals->getPtr(widened ? reader->u2L() : reader->u1());
        SE8::Value* value = stack->pop();
        locals->setType(ptr, value->type);
        if (value->type == SE8::Float) {
            locals->set32(ptr, *(uint32_t*)(&value->bytes));
        }
        widened = false;
    }

    void Frame::fstore_0() {
        uint64_t ptr = locals->getPtr(0);
        SE8::Value* value = stack->pop();
        locals->setType(ptr, value->type);
        if (value->type == SE8::Float) {
            locals->set32(ptr, *(uint32_t*)(&value->bytes));
        }
        widened = false;
    }

    void Frame::fstore_1() {
        uint64_t ptr = locals->getPtr(1);
        SE8::Value* value = stack->pop();
        locals->setType(ptr, value->type);
        if (value->type == SE8::Float) {
            locals->set32(ptr, *(uint32_t*)(&value->bytes));
        }
        widened = false;
    }

    void Frame::fstore_2() {
        uint64_t ptr = locals->getPtr(2);
        SE8::Value* value = stack->pop();
        locals->setType(ptr, value->type);
        if (value->type == SE8::Float) {
            locals->set32(ptr, *(uint32_t*)(&value->bytes));
        }
        widened = false;
    }

    void Frame::fstore_3() {
        uint64_t ptr = locals->getPtr(3);
        SE8::Value* value = stack->pop();
        locals->setType(ptr, value->type);
        if (value->type == SE8::Float) {
            locals->set32(ptr, *(uint32_t*)(&value->bytes));
        }
        widened = false;
    }

    void Frame::fsub() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::Float && value2->type == SE8::Float) {
            float val1 = *(uint32_t*)((uint64_t) value1 + 1);
            float val2 = *(uint32_t*)((uint64_t) value2 + 1);
            stack->pushFloat(val1 - val2);
        } else {
            // throw type exception
        }
    }

    void Frame::getfield() {

    }

    void Frame::getstatic() {

    }

    void Frame::goto_() {

    }

    void Frame::goto_w() {

    }

    void Frame::i2b() {
        SE8::Value* value = stack->pop();
        if (value->type == SE8::Int) {
            int32_t val = *(uint32_t*)((uint64_t) value + 1);
            stack->pushByte(static_cast<uint8_t>(val));
        } else {
            // throw type exception
        }
    }

    void Frame::i2c() {
        SE8::Value* value = stack->pop();
        if (value->type == SE8::Int) {
            int32_t val = *(uint32_t*)((uint64_t) value + 1);
            stack->pushChar(static_cast<uint16_t>(val));
        } else {
            // throw type exception
        }
    }

    void Frame::i2d() {
        SE8::Value* value = stack->pop();
        if (value->type == SE8::Int) {
            int32_t val = *(uint32_t*)((uint64_t) value + 1);
            stack->pushDouble(static_cast<double>(val));
        } else {
            // throw type exception
        }
    }

    void Frame::i2f() {
        SE8::Value* value = stack->pop();
        if (value->type == SE8::Int) {
            int32_t val = *(uint32_t*)((uint64_t) value + 1);
            stack->pushFloat(static_cast<float>(val));
        } else {
            // throw type exception
        }
    }

    void Frame::i2l() {
        SE8::Value* value = stack->pop();
        if (value->type == SE8::Int) {
            int32_t val = *(uint32_t*)((uint64_t) value + 1);
            stack->pushLong(static_cast<int64_t>(val));
        } else {
            // throw type exception
        }
    }

    void Frame::i2s() {
        SE8::Value* value = stack->pop();
        if (value->type == SE8::Int) {
            int32_t val = *(uint32_t*)((uint64_t) value + 1);
            stack->pushShort(static_cast<int16_t>(val));
        } else {
            // throw type exception
        }
    }

    void Frame::iadd() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::Int && value2->type == SE8::Int) {
            int32_t val1 = *(uint32_t*)((uint64_t) value1 + 1);
            int32_t val2 = *(uint32_t*)((uint64_t) value2 + 1);
            stack->pushInt(val1 + val2);
        } else {
            // throw type exception
        }
    }

    void Frame::iaload() {

    }

    void Frame::iand() {

    }

    void Frame::iastore() {

    }

    void Frame::iconst_m1() {
        stack->pushInt(-1);
    }

    void Frame::iconst_0() {
        stack->pushInt(0);
    }

    void Frame::iconst_1() {
        stack->pushInt(1);
    }

    void Frame::iconst_2() {
        stack->pushInt(2);
    }

    void Frame::iconst_3() {
        stack->pushInt(3);
    }

    void Frame::iconst_4() {
        stack->pushInt(4);
    }

    void Frame::iconst_5() {
        stack->pushInt(5);
    }

    void Frame::idiv() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::Int && value2->type == SE8::Int) {
            int val2 = *(uint32_t*)((uint64_t) value2 + 1);
            if (val2 == 0) {
                stack->pushNaN();
            } else {
                int val1 = *(uint32_t*)((uint64_t) value1 + 1);
                stack->pushInt(val1 / val2);
            }
        } else {
            // throw type exception
        }
    }

    void Frame::if_acmpeq() {

    }

    void Frame::if_acmpne() {

    }

    void Frame::if_icmpeq() {

    }

    void Frame::if_icmpne() {

    }

    void Frame::if_icmplt() {

    }

    void Frame::if_icmpge() {

    }

    void Frame::if_icmpgt() {

    }

    void Frame::if_icmple() {

    }

    void Frame::ifeq() {

    }

    void Frame::ifne() {

    }

    void Frame::iflt() {

    }

    void Frame::ifge() {

    }

    void Frame::ifgt() {

    }

    void Frame::ifle() {

    }

    void Frame::ifnonnull() {

    }

    void Frame::ifnull() {

    }

    void Frame::iinc() {

    }

    void Frame::iload() {
        uint64_t ptr = locals->getPtr(widened ? reader->u2L() : reader->u1());
        uint8_t type = locals->getType(ptr);
        if (type == SE8::Float) {
            stack->pushInt(locals->get32(ptr));
        } else if (type == SE8::NaN) {
            stack->pushNaN();
        } else if (type == SE8::Null) {
            stack->pushNull();
        } else {
            // throw type exception
        }
        widened = false;
    }

    void Frame::iload_0() {
        uint64_t ptr = locals->getPtr(0);
        uint8_t type = locals->getType(ptr);
        if (type == SE8::Float) {
            stack->pushInt(locals->get32(ptr));
        } else if (type == SE8::NaN) {
            stack->pushNaN();
        } else if (type == SE8::Null) {
            stack->pushNull();
        } else {
            // throw type exception
        }
        widened = false;
    }

    void Frame::iload_1() {
        uint64_t ptr = locals->getPtr(1);
        uint8_t type = locals->getType(ptr);
        if (type == SE8::Float) {
            stack->pushInt(locals->get32(ptr));
        } else if (type == SE8::NaN) {
            stack->pushNaN();
        } else if (type == SE8::Null) {
            stack->pushNull();
        } else {
            // throw type exception
        }
        widened = false;
    }

    void Frame::iload_2() {
        uint64_t ptr = locals->getPtr(2);
        uint8_t type = locals->getType(ptr);
        if (type == SE8::Float) {
            stack->pushInt(locals->get32(ptr));
        } else if (type == SE8::NaN) {
            stack->pushNaN();
        } else if (type == SE8::Null) {
            stack->pushNull();
        } else {
            // throw type exception
        }
        widened = false;
    }

    void Frame::iload_3() {
        uint64_t ptr = locals->getPtr(3);
        uint8_t type = locals->getType(ptr);
        if (type == SE8::Float) {
            stack->pushInt(locals->get32(ptr));
        } else if (type == SE8::NaN) {
            stack->pushNaN();
        } else if (type == SE8::Null) {
            stack->pushNull();
        } else {
            // throw type exception
        }
        widened = false;
    }

    void Frame::imul() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::Int && value2->type == SE8::Int) {
            int32_t val1 = *(uint32_t*)((uint64_t) value1 + 1);
            int32_t val2 = *(uint32_t*)((uint64_t) value2 + 1);
            stack->pushInt(val1 * val2);
        } else {
            // throw type exception
        }
    }

    void Frame::ineg() {
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::Int) {
            int32_t val1 = *(uint32_t*)((uint64_t) value1 + 1);
            stack->pushInt(-val1);
        } else {
            // throw type exception
        }
    }

    void Frame::instanceof() {

    }

    void Frame::invokedynamic() {

    }

    void Frame::invokeinterface() {

    }

    void Frame::invokespecial() {

    }

    void Frame::invokestatic() {

    }

    void Frame::invokevirtual() {

    }

    void Frame::ior() {

    }

    void Frame::irem() {

    }

    void Frame::ireturn() {

    }

    void Frame::ishl() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::Int && value2->type == SE8::Int) {
            int32_t val1 = *(uint32_t*)((uint64_t) value1 + 1);
            int64_t val2 = *(uint64_t*)((uint64_t) value2 + 1);
            stack->pushInt(val1 << val2);
        } else if (value1->type == SE8::NaN || value2->type == SE8::NaN) {
            stack->pushNaN();
        } else if (value1->type == SE8::Null || value2->type == SE8::Null) {
            stack->pushNull();
        } else {
            // type exception
        }
    }

    void Frame::ishr() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::Int && value2->type == SE8::Int) {
            int32_t val1 = *(uint32_t*)((uint64_t) value1 + 1);
            int64_t val2 = *(uint64_t*)((uint64_t) value2 + 1);
            stack->pushInt(val1 >> val2);
        } else if (value1->type == SE8::NaN || value2->type == SE8::NaN) {
            stack->pushNaN();
        } else if (value1->type == SE8::Null || value2->type == SE8::Null) {
            stack->pushNull();
        } else {
            // type exception
        }
    }

    void Frame::istore() {
        uint64_t ptr = locals->getPtr(widened ? reader->u2L() : reader->u1());
        SE8::Value* value = stack->pop();
        locals->setType(ptr, value->type);
        if (value->type == SE8::Int) {
            locals->set32(ptr, *(uint32_t*)(&value->bytes));
        }
        widened = false;
    }

    void Frame::istore_0() {
        uint64_t ptr = locals->getPtr(0);
        SE8::Value* value = stack->pop();
        locals->setType(ptr, value->type);
        if (value->type == SE8::Int) {
            locals->set32(ptr, *(uint32_t*)(&value->bytes));
        }
        widened = false;
    }

    void Frame::istore_1() {
        uint64_t ptr = locals->getPtr(1);
        SE8::Value* value = stack->pop();
        locals->setType(ptr, value->type);
        if (value->type == SE8::Int) {
            locals->set32(ptr, *(uint32_t*)(&value->bytes));
        }
        widened = false;
    }

    void Frame::istore_2() {
        uint64_t ptr = locals->getPtr(2);
        SE8::Value* value = stack->pop();
        locals->setType(ptr, value->type);
        if (value->type == SE8::Int) {
            locals->set32(ptr, *(uint32_t*)(&value->bytes));
        }
        widened = false;
    }

    void Frame::istore_3() {
        uint64_t ptr = locals->getPtr(3);
        SE8::Value* value = stack->pop();
        locals->setType(ptr, value->type);
        if (value->type == SE8::Int) {
            locals->set32(ptr, *(uint32_t*)(&value->bytes));
        }
        widened = false;
    }

    void Frame::isub() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::Int && value2->type == SE8::Int) {
            int32_t val1 = *(uint32_t*)((uint64_t) value1 + 1);
            int32_t val2 = *(uint32_t*)((uint64_t) value2 + 1);
            stack->pushInt(val1 - val2);
        } else {
            // throw type exception
        }
    }

    void Frame::iushr() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::Int && value2->type == SE8::Int) {
            int32_t val1 = *(uint32_t*)((uint64_t) value1 + 1);
            int64_t val2 = *(uint64_t*)((uint64_t) value2 + 1);
            int32_t mask = 0x7fffffff;
            mask = mask >> val2;
            mask = (mask << 1) | 1;
            stack->pushLong((val1 >> val2) & mask);
        } else if (value1->type == SE8::NaN || value2->type == SE8::NaN) {
            stack->pushNaN();
        } else if (value1->type == SE8::Null || value2->type == SE8::Null) {
            stack->pushNull();
        } else {
            // type exception
        }
    }

    void Frame::ixor() {

    }

    void Frame::jsr() {
        uint16_t jmp = reader->u2L();
        stack->pushInt(reader->index);
        reader->index = jmp;
    }

    void Frame::jsr_w() {
        uint32_t jmp = reader->u4L();
        stack->pushInt(reader->index);
        reader->index = jmp;
    }

    void Frame::l2d() {

    }

    void Frame::l2f() {

    }

    void Frame::l2i() {

    }

    void Frame::ladd() {

    }

    void Frame::laload() {

    }

    void Frame::land() {

    }

    void Frame::lastore() {

    }

    void Frame::lcmp() {

    }

    void Frame::lconst_0() {
        stack->pushLong(0);
    }

    void Frame::lconst_1() {
        stack->pushLong(1);
    }

    void Frame::ldc() {

    }

    void Frame::ldc_w() {

    }

    void Frame::ldc2_w() {

    }

    void Frame::ldiv() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::Long && value2->type == SE8::Long) {
            int64_t val2 = *(uint64_t*)((uint64_t) value2 + 1);
            if (val2 == 0) {
                stack->pushNaN();
            } else {
                int64_t val1 = *(uint64_t*)((uint64_t) value1 + 1);
                stack->pushLong(val1 / val2);
            }
        } else {
            // throw type exception
        }
    }

    void Frame::lload() {
        uint64_t ptr = locals->getPtr(widened ? reader->u2L() : reader->u1());
        uint8_t type = locals->getType(ptr);
        if (type == SE8::Long) {
            stack->pushLong(locals->get64(ptr));
        } else if (type == SE8::NaN) {
            stack->pushNaN();
        } else if (type == SE8::Null) {
            stack->pushNull();
        } else {
            // throw type exception
        }
        widened = false;
    }

    void Frame::lload_0() {
        uint64_t ptr = locals->getPtr(0);
        uint8_t type = locals->getType(ptr);
        if (type == SE8::Long) {
            stack->pushLong(locals->get64(ptr));
        } else if (type == SE8::NaN) {
            stack->pushNaN();
        } else if (type == SE8::Null) {
            stack->pushNull();
        } else {
            // throw type exception
        }
        widened = false;
    }

    void Frame::lload_1() {
        uint64_t ptr = locals->getPtr(1);
        uint8_t type = locals->getType(ptr);
        if (type == SE8::Long) {
            stack->pushLong(locals->get64(ptr));
        } else if (type == SE8::NaN) {
            stack->pushNaN();
        } else if (type == SE8::Null) {
            stack->pushNull();
        } else {
            // throw type exception
        }
        widened = false;
    }

    void Frame::lload_2() {
        uint64_t ptr = locals->getPtr(2);
        uint8_t type = locals->getType(ptr);
        if (type == SE8::Long) {
            stack->pushLong(locals->get64(ptr));
        } else if (type == SE8::NaN) {
            stack->pushNaN();
        } else if (type == SE8::Null) {
            stack->pushNull();
        } else {
            // throw type exception
        }
        widened = false;
    }

    void Frame::lload_3() {
        uint64_t ptr = locals->getPtr(3);
        uint8_t type = locals->getType(ptr);
        if (type == SE8::Long) {
            stack->pushLong(locals->get64(ptr));
        } else if (type == SE8::NaN) {
            stack->pushNaN();
        } else if (type == SE8::Null) {
            stack->pushNull();
        } else {
            // throw type exception
        }
        widened = false;
    }

    void Frame::lmul() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::Long && value2->type == SE8::Long) {
            int64_t val1 = *(uint64_t*)((uint64_t) value1 + 1);
            int64_t val2 = *(uint64_t*)((uint64_t) value2 + 1);
            stack->pushLong(val1 * val2);
        } else {
            // throw type exception
        }
    }

    void Frame::lneg() {
        SE8::Value* value = stack->pop();
        if (value->type == SE8::Long) {
            int64_t val = *(uint64_t*)((uint64_t) value + 1);
            stack->pushLong(-val);
        } else {
            // throw type exception
        }
    }

    void Frame::lookupswitch() {

    }

    void Frame::lor() {

    }

    void Frame::lrem() {

    }

    void Frame::lreturn() {

    }

    void Frame::lshl() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::Long && value2->type == SE8::Int) {
            int32_t val1 = *(uint32_t*)((uint64_t) value1 + 1);
            int64_t val2 = *(uint64_t*)((uint64_t) value2 + 1);
            stack->pushLong(val1 << val2);
        } else if (value1->type == SE8::NaN || value2->type == SE8::NaN) {
            stack->pushNaN();
        } else if (value1->type == SE8::Null || value2->type == SE8::Null) {
            stack->pushNull();
        } else {
            // type exception
        }
    }

    void Frame::lshr() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::Long && value2->type == SE8::Int) {
            int32_t val1 = *(uint32_t*)((uint64_t) value1 + 1);
            int64_t val2 = *(uint64_t*)((uint64_t) value2 + 1);
            stack->pushLong(val1 >> val2);
        } else if (value1->type == SE8::NaN || value2->type == SE8::NaN) {
            stack->pushNaN();
        } else if (value1->type == SE8::Null || value2->type == SE8::Null) {
            stack->pushNull();
        } else {
            // type exception
        }
    }

    void Frame::lstore() {
        uint64_t ptr = locals->getPtr(widened ? reader->u2L() : reader->u1());
        SE8::Value* value = stack->pop();
        locals->setType(ptr, value->type);
        if (value->type == SE8::Long) {
            locals->set64(ptr, *(uint64_t*)(&value->bytes));
        }
        widened = false;
    }

    void Frame::lstore_0() {
        uint64_t ptr = locals->getPtr(0);
        SE8::Value* value = stack->pop();
        locals->setType(ptr, value->type);
        if (value->type == SE8::Long) {
            locals->set64(ptr, *(uint64_t*)(&value->bytes));
        }
        widened = false;
    }

    void Frame::lstore_1() {
        uint64_t ptr = locals->getPtr(1);
        SE8::Value* value = stack->pop();
        locals->setType(ptr, value->type);
        if (value->type == SE8::Long) {
            locals->set64(ptr, *(uint64_t*)(&value->bytes));
        }
        widened = false;
    }

    void Frame::lstore_2() {
        uint64_t ptr = locals->getPtr(2);
        SE8::Value* value = stack->pop();
        locals->setType(ptr, value->type);
        if (value->type == SE8::Long) {
            locals->set64(ptr, *(uint64_t*)(&value->bytes));
        }
        widened = false;
    }

    void Frame::lstore_3() {
        uint64_t ptr = locals->getPtr(3);
        SE8::Value* value = stack->pop();
        locals->setType(ptr, value->type);
        if (value->type == SE8::Long) {
            locals->set64(ptr, *(uint64_t*)(&value->bytes));
        }
        widened = false;
    }

    void Frame::lsub() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::Long && value2->type == SE8::Long) {
            int64_t val1 = *(uint64_t*)((uint64_t) value1 + 1);
            int64_t val2 = *(uint64_t*)((uint64_t) value2 + 1);
            stack->pushDouble(val1 - val2);
        } else {
            // throw type exception
        }
    }

    void Frame::lushr() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        if (value1->type == SE8::Long && value2->type == SE8::Int) {
            int64_t val1 = *(uint64_t*)((uint64_t) value1 + 1);
            int64_t val2 = *(uint64_t*)((uint64_t) value2 + 1);
            int64_t mask = 0x7fffffffffffffff;
            mask = mask >> val2;
            mask = (mask << 1) | 1;
            stack->pushLong((val1 >> val2) & mask);
        } else if (value1->type == SE8::NaN || value2->type == SE8::NaN) {
            stack->pushNaN();
        } else if (value1->type == SE8::Null || value2->type == SE8::Null) {
            stack->pushNull();
        } else {
            // type exception
        }
    }

    void Frame::lxor() {

    }

    void Frame::monitorenter() {

    }

    void Frame::monitorexit() {

    }

    void Frame::multianewarray() {

    }

    void Frame::new_() {

    }

    void Frame::newarray() {

    }

    void Frame::nop() {
        // do nothing
    }

    void Frame::pop() {
        stack->wpop();
    }

    void Frame::pop2() {
        stack->wpop();
    }

    void Frame::putfield() {

    }

    void Frame::putstatic() {

    }

    void Frame::ret() {

    }

    void Frame::return_() {

    }

    void Frame::saload() {

    }

    void Frame::sastore() {

    }

    void Frame::sipush() {

    }

    void Frame::swap() {
        SE8::Value* val1 = stack->pop();
        SE8::Value* val2 = stack->pop();
        stack->push(val1);
        stack->push(val2);
    }

    void Frame::tableswitch() {

    }

    void Frame::wide() {
        this->widened = true;
    }

}
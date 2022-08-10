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
        this->local_variable = new LocalVariable();
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
        double val1 = *(uint64_t*)((uint64_t) value1 + 1);
        double val2 = *(uint64_t*)((uint64_t) value2 + 1);
        if (value1->type == SE8::Double && value2->type == SE8::Double) {
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
        double val1 = *(uint64_t*)((uint64_t) value1 + 1);
        double val2 = *(uint64_t*)((uint64_t) value2 + 1);
        if (value1->type == SE8::NaN || value2->type == SE8::NaN) {
            stack->pushInt(1);
        } else if (value1->type == SE8::Double && value2->type == SE8::Double) {
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
        double val1 = *(uint64_t*)((uint64_t) value1 + 1);
        double val2 = *(uint64_t*)((uint64_t) value2 + 1);
        if (value1->type == SE8::NaN || value2->type == SE8::NaN) {
            stack->pushInt(-1);
        } else if (value1->type == SE8::Double && value2->type == SE8::Double) {
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
        double val1 = *(uint64_t*)((uint64_t) value1 + 1);
        double val2 = *(uint64_t*)((uint64_t) value2 + 1);
        if (value1->type == SE8::Double && value2->type == SE8::Double) {
            if (val2 == 0) {
                stack->pushNaN();
            } else {
                stack->pushDouble(val1 / val2);
            }
        } else {
            // throw type exception
        }
    }

    void Frame::dload() {

    }

    void Frame::dload_0() {

    }

    void Frame::dload_1() {

    }

    void Frame::dload_2() {

    }

    void Frame::dload_3() {

    }

    void Frame::dmul() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        double val1 = *(uint64_t*)((uint64_t) value1 + 1);
        double val2 = *(uint64_t*)((uint64_t) value2 + 1);
        if (value1->type == SE8::Double && value2->type == SE8::Double) {
            stack->pushDouble(val1 * val2);
        } else {
            // throw type exception
        }
    }

    void Frame::dneg() {
        SE8::Value* value = stack->pop();
        double val = *(uint64_t*)((uint64_t) value + 1);
        if (value->type == SE8::Double) {
            stack->pushDouble(-val);
        } else {
            // throw type exception
        }
    }

    void Frame::drem() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        double val1 = *(uint64_t*)((uint64_t) value1 + 1);
        double val2 = *(uint64_t*)((uint64_t) value2 + 1);
        if (value1->type == SE8::Double && value2->type == SE8::Double) {
            stack->pushDouble(val1 * val2);
        } else {
            // throw type exception
        }
    }

    void Frame::dreturn() {

    }

    void Frame::dstore() {

    }

    void Frame::dstore_0() {

    }

    void Frame::dstore_1() {

    }

    void Frame::dstore_2() {

    }

    void Frame::dstore_3() {

    }

    void Frame::dsub() {
        SE8::Value* value2 = stack->pop();
        SE8::Value* value1 = stack->pop();
        double val1 = *(uint64_t*)((uint64_t) value1 + 1);
        double val2 = *(uint64_t*)((uint64_t) value2 + 1);
        if (value1->type == SE8::Double && value2->type == SE8::Double) {
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
        SE8::Value* val2 = stack->peek();
        if (getComputationalCategory(val1->type) == 2) {
            stack->push(val1);
            stack->push(val1);
        } else if (getComputationalCategory(val1->type) == 1 && getComputationalCategory(val2->type) == 1) {
            stack->push(val1);
            stack->push(val2);
            stack->push(val1);
        }
    }

    void Frame::dup2_x1() {
        SE8::Value* val1 = stack->pop();
        SE8::Value* val2 = stack->pop();
        if (getComputationalCategory(val1->type) == 1) {
            SE8::Value* val3 = stack->pop();
            stack->push(val2);
            stack->push(val1);
            stack->push(val3);
            stack->push(val2);
            stack->push(val1);
        } else if (getComputationalCategory(val2->type) == 1) {
            stack->push(val1);
            stack->push(val2);
            stack->push(val1);
        }
    }

    void Frame::dup2_x2() {
        // todo tomorrow
    }

    void Frame::f2d() {

    }

    void Frame::f2i() {

    }

    void Frame::f2l() {

    }

    void Frame::fadd() {

    }

    void Frame::faload() {

    }

    void Frame::fastore() {

    }

    void Frame::fcmpg() {

    }

    void Frame::fcpml() {

    }

    void Frame::fconst_0() {

    }

    void Frame::fconst_1() {

    }

    void Frame::fconst_2() {

    }

    void Frame::fdiv() {

    }

    void Frame::fload() {

    }

    void Frame::fload_0() {

    }

    void Frame::fload_1() {

    }

    void Frame::fload_2() {

    }

    void Frame::fload_3() {

    }

    void Frame::fmul() {

    }

    void Frame::fneg() {

    }

    void Frame::frem() {

    }

    void Frame::freturn() {

    }

    void Frame::fstore() {

    }

    void Frame::fstore_0() {

    }

    void Frame::fstore_1() {

    }

    void Frame::fstore_2() {

    }

    void Frame::fstore_3() {

    }

    void Frame::fsub() {

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

    }

    void Frame::i2c() {

    }

    void Frame::i2d() {

    }

    void Frame::i2f() {

    }

    void Frame::i2l() {

    }

    void Frame::i2s() {

    }

    void Frame::iadd() {

    }

    void Frame::iaload() {

    }

    void Frame::iand() {

    }

    void Frame::iastore() {

    }

    void Frame::iconst_m1() {

    }

    void Frame::iconst_0() {

    }

    void Frame::iconst_1() {

    }

    void Frame::iconst_2() {

    }

    void Frame::iconst_4() {

    }

    void Frame::iconst_5() {

    }

    void Frame::idiv() {

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

    }

    void Frame::iload_0() {

    }

    void Frame::iload_1() {

    }

    void Frame::iload_2() {

    }

    void Frame::iload_3() {

    }

    void Frame::imul() {

    }

    void Frame::ineg() {

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

    }

    void Frame::ishr() {

    }

    void Frame::istore() {

    }

    void Frame::istore_0() {

    }

    void Frame::istore_1() {

    }

    void Frame::istore_2() {

    }

    void Frame::istore_3() {

    }

    void Frame::isub() {

    }

    void Frame::iushr() {

    }

    void Frame::ixor() {

    }

    void Frame::jsr() {

    }

    void Frame::jsr_w() {

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

    }

    void Frame::lconst_1() {

    }

    void Frame::ldc() {

    }

    void Frame::ldc_w() {

    }

    void Frame::ldc2_w() {

    }

    void Frame::ldiv() {

    }

    void Frame::lload() {

    }

    void Frame::lload_0() {

    }

    void Frame::lload_1() {

    }

    void Frame::lload_2() {

    }

    void Frame::lload_3() {

    }

    void Frame::lmul() {

    }

    void Frame::lneg() {

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

    }

    void Frame::lshr() {

    }

    void Frame::lstore() {

    }

    void Frame::lstore_0() {

    }

    void Frame::lstore_1() {

    }

    void Frame::lstore_2() {

    }

    void Frame::lstore_3() {

    }

    void Frame::lsub() {

    }

    void Frame::lushr() {

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

    }

    void Frame::pop() {

    }

    void Frame::pop2() {

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

    }

    void Frame::tableswitch() {

    }

    void Frame::wide() {

    }

}
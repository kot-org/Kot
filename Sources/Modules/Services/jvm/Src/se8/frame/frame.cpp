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

    void Frame::init(Class* cl, Method* method) {
        currentClass = cl;
        currentMethod = method;
        reader = (Reader*) malloc(sizeof(Reader));
        for (uint16_t i = 0; i < currentMethod->attributes_count; i++) {
            Attribute_Code* attr = (Attribute_Code*) currentMethod->attributes[i];
            if (attr->attribute_type == AT_Code) {
                stack = new Stack(attr->max_stack*5);
                locals = new Locals(attr->max_locals);
                reader->buffer = attr->code;
                code_length = attr->code_length;
            }
        }
    }

    void Opc::aaload(Frame* frame) {

    }

    void Opc::aastore(Frame* frame) {

    }

    void Opc::aconst_null(Frame* frame) {
        frame->stack->pushNull();
    }

    void Opc::aload(Frame* frame) {

    }

    void Opc::aload_0(Frame* frame) {

    }

    void Opc::aload_1(Frame* frame) {

    }

    void Opc::aload_2(Frame* frame) {

    }

    void Opc::aload_3(Frame* frame) {

    }

    void Opc::anewarray(Frame* frame) { 

    }

    void Opc::areturn(Frame* frame) {

    }

    void Opc::arraylength(Frame* frame) {

    }

    void Opc::astore(Frame* frame) {

    }

    void Opc::astore_0(Frame* frame) {

    }

    void Opc::astore_1(Frame* frame) {

    }

    void Opc::astore_2(Frame* frame) {

    }

    void Opc::astore_3(Frame* frame) {

    }

    void Opc::athrow(Frame* frame) {

    }

    void Opc::baload(Frame* frame) {

    }

    void Opc::bastore(Frame* frame) {

    }

    void Opc::bipush(Frame* frame) {
        frame->stack->pushByte(frame->reader->u1());
    }

    void Opc::caload(Frame* frame) {

    }

    void Opc::castore(Frame* frame) {

    }

    void Opc::checkcast(Frame* frame) {

    }

    void Opc::d2f(Frame* frame) {
        double val = *(uint64_t*)((uint64_t) frame->stack->pop() + 1);
        frame->stack->pushFloat(static_cast<float>(val));
    }

    void Opc::d2i(Frame* frame) {
        double val = *(uint64_t*)((uint64_t) frame->stack->pop() + 1);
        frame->stack->pushInt(static_cast<int32_t>(val));
    }

    void Opc::d2l(Frame* frame) {
        double val = *(uint64_t*)((uint64_t) frame->stack->pop() + 1);
        frame->stack->pushLong(static_cast<int64_t>(val));
    }

    void Opc::dadd(Frame* frame) {
        SE8::Value* value2 = frame->stack->pop();
        SE8::Value* value1 = frame->stack->pop();
        double val1 = *(uint64_t*)((uint64_t) value1 + 1);
        double val2 = *(uint64_t*)((uint64_t) value2 + 1);
        frame->stack->pushDouble(val1 + val2);
    }

    void Opc::daload(Frame* frame) {

    }

    void Opc::dastore(Frame* frame) {
        
    }

    void Opc::dcmpg(Frame* frame) {
        SE8::Value* value2 = frame->stack->pop();
        SE8::Value* value1 = frame->stack->pop();
        if (value1->type == SE8::NaN || value2->type == SE8::NaN) {
            frame->stack->pushInt(1);
        } else if (value1->type == SE8::Double && value2->type == SE8::Double) {
            double val1 = *(uint64_t*)((uint64_t) value1 + 1);
            double val2 = *(uint64_t*)((uint64_t) value2 + 1);
            if (val1 == val2) {
                frame->stack->pushInt(0);
            } else if (val1 > val2) {
                frame->stack->pushInt(1);
            } else if (val1 < val2) {
                frame->stack->pushInt(-1);
            }
        }
    }

    void Opc::dcmpl(Frame* frame) {
        SE8::Value* value2 = frame->stack->pop();
        SE8::Value* value1 = frame->stack->pop();
        if (value1->type == SE8::NaN || value2->type == SE8::NaN) {
            frame->stack->pushInt(-1);
        } else if (value1->type == SE8::Double && value2->type == SE8::Double) {
            double val1 = *(uint64_t*)((uint64_t) value1 + 1);
            double val2 = *(uint64_t*)((uint64_t) value2 + 1);
            if (val1 == val2) {
                frame->stack->pushInt(0);
            } else if (val1 > val2) {
                frame->stack->pushInt(1);
            } else if (val1 < val2) {
                frame->stack->pushInt(-1);
            }
        }
    }

    void Opc::dconst_0(Frame* frame) {
        frame->stack->pushDouble(0.0);
    }

    void Opc::dconst_1(Frame* frame) {
        frame->stack->pushDouble(1.0);
    }

    void Opc::ddiv(Frame* frame) {
        SE8::Value* value2 = frame->stack->pop();
        SE8::Value* value1 = frame->stack->pop();
        if (value1->type == SE8::Double && value2->type == SE8::Double) {
            double val2 = *(uint64_t*)((uint64_t) value2 + 1);
            if (val2 == 0) {
                // throw arithemic exception
            } else {
                double val1 = *(uint64_t*)((uint64_t) value1 + 1);
                frame->stack->pushDouble(val1 / val2);
            }
        }
    }

    void Opc::dload(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(frame->widened ? frame->reader->u2B() : frame->reader->u1());
        uint8_t type = frame->locals->getType(ptr);
        if (type == SE8::Double) {
            frame->stack->pushDouble(frame->locals->get64(ptr));
        } else if (type == SE8::NaN) {
            frame->stack->pushNaN();
        } else if (type == SE8::Null) {
            frame->stack->pushNull();
        }
        frame->widened = false;
    }

    void Opc::dload_0(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(0);
        uint8_t type = frame->locals->getType(ptr);
        if (type == SE8::Double) {
            frame->stack->pushDouble(frame->locals->get64(ptr));
        } else if (type == SE8::NaN) {
            frame->stack->pushNaN();
        } else if (type == SE8::Null) {
            frame->stack->pushNull();
        }
        frame->widened = false;
    }

    void Opc::dload_1(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(1);
        uint8_t type = frame->locals->getType(ptr);
        if (type == SE8::Double) {
            frame->stack->pushDouble(frame->locals->get64(ptr));
        } else if (type == SE8::NaN) {
            frame->stack->pushNaN();
        } else if (type == SE8::Null) {
            frame->stack->pushNull();
        }
        frame->widened = false;
    }

    void Opc::dload_2(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(2);
        uint8_t type = frame->locals->getType(ptr);
        if (type == SE8::Double) {
            frame->stack->pushDouble(frame->locals->get64(ptr));
        } else if (type == SE8::NaN) {
            frame->stack->pushNaN();
        } else if (type == SE8::Null) {
            frame->stack->pushNull();
        }
        frame->widened = false;
    }

    void Opc::dload_3(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(3);
        uint8_t type = frame->locals->getType(ptr);
        if (type == SE8::Double) {
            frame->stack->pushDouble(frame->locals->get64(ptr));
        } else if (type == SE8::NaN) {
            frame->stack->pushNaN();
        } else if (type == SE8::Null) {
            frame->stack->pushNull();
        }
        frame->widened = false;
    }

    void Opc::dmul(Frame* frame) {
        SE8::Value* value2 = frame->stack->pop();
        SE8::Value* value1 = frame->stack->pop();
        if (value1->type == SE8::Double && value2->type == SE8::Double) {
            double val1 = *(uint64_t*)((uint64_t) value1 + 1);
            double val2 = *(uint64_t*)((uint64_t) value2 + 1);
            frame->stack->pushDouble(val1 * val2);
        }
    }

    void Opc::dneg(Frame* frame) {
        double val = *(uint64_t*)((uint64_t) frame->stack->pop() + 1);
        frame->stack->pushDouble(-val);
    }

    void Opc::drem(Frame* frame) {
        SE8::Value* value2 = frame->stack->pop();
        SE8::Value* value1 = frame->stack->pop();
        if (value1->type == SE8::Double && value2->type == SE8::Double) {
            double val1 = *(uint64_t*)((uint64_t) value1 + 1);
            double val2 = *(uint64_t*)((uint64_t) value2 + 1);
            frame->stack->pushDouble(val1 * val2);
        }
    }

    void Opc::dreturn(Frame* frame) {

    }

    void Opc::dstore(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(frame->widened ? frame->reader->u2B() : frame->reader->u1());
        SE8::Value* value = frame->stack->pop();
        frame->locals->setType(ptr, value->type);
        if (value->type == SE8::Double) {
            frame->locals->set64(ptr, *(uint64_t*)(&value->bytes));
        }
        frame->widened = false;
    }

    void Opc::dstore_0(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(0);
        SE8::Value* value = frame->stack->pop();
        frame->locals->setType(ptr, value->type);
        if (value->type == SE8::Double) {
            frame->locals->set64(ptr, *(uint64_t*)(&value->bytes));
        }
        frame->widened = false;
    }

    void Opc::dstore_1(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(1);
        SE8::Value* value = frame->stack->pop();
        frame->locals->setType(ptr, value->type);
        if (value->type == SE8::Double) {
            frame->locals->set64(ptr, *(uint64_t*)(&value->bytes));
        }
        frame->widened = false;
    }

    void Opc::dstore_2(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(2);
        SE8::Value* value = frame->stack->pop();
        frame->locals->setType(ptr, value->type);
        if (value->type == SE8::Double) {
            frame->locals->set64(ptr, *(uint64_t*)(&value->bytes));
        }
        frame->widened = false;
    }

    void Opc::dstore_3(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(3);
        SE8::Value* value = frame->stack->pop();
        frame->locals->setType(ptr, value->type);
        if (value->type == SE8::Double) {
            frame->locals->set64(ptr, *(uint64_t*)(&value->bytes));
        }
        frame->widened = false;
    }

    void Opc::dsub(Frame* frame) {
        SE8::Value* value2 = frame->stack->pop();
        SE8::Value* value1 = frame->stack->pop();
        if (value1->type == SE8::Double && value2->type == SE8::Double) {
            double val1 = *(uint64_t*)((uint64_t) value1 + 1);
            double val2 = *(uint64_t*)((uint64_t) value2 + 1);
            frame->stack->pushDouble(val1 - val2);
        }
    }

    void Opc::dup(Frame* frame) {
        frame->stack->push(frame->stack->peek());
    }

    void Opc::dup_x1(Frame* frame) {
        SE8::Value* val1 = frame->stack->pop();
        SE8::Value* val2 = frame->stack->pop();
        frame->stack->push(val1);
        frame->stack->push(val2);
        frame->stack->push(val1);
    }

    void Opc::dup_x2(Frame* frame) {
        SE8::Value* val1 = frame->stack->pop();
        SE8::Value* val2 = frame->stack->pop();
        SE8::Value* val3 = frame->stack->pop();
        frame->stack->push(val1);
        frame->stack->push(val3);
        frame->stack->push(val2);
        frame->stack->push(val1);
    }

    void Opc::dup2(Frame* frame) {
        SE8::Value* val1 = frame->stack->pop();
        SE8::Value* val2 = frame->stack->pop();
        frame->stack->push(val2);
        frame->stack->push(val1);
        frame->stack->push(val2);
        frame->stack->push(val1);
    }

    void Opc::dup2_x1(Frame* frame) {
        SE8::Value* val1 = frame->stack->pop();
        SE8::Value* val2 = frame->stack->pop();
        SE8::Value* val3 = frame->stack->pop();
        frame->stack->push(val2);
        frame->stack->push(val1);
        frame->stack->push(val3);
        frame->stack->push(val2);
        frame->stack->push(val1);
    }

    void Opc::dup2_x2(Frame* frame) {
        SE8::Value* val1 = frame->stack->pop();
        SE8::Value* val2 = frame->stack->pop();
        SE8::Value* val3 = frame->stack->pop();
        SE8::Value* val4 = frame->stack->pop();
        frame->stack->push(val2);
        frame->stack->push(val1);
        frame->stack->push(val4);
        frame->stack->push(val3);
        frame->stack->push(val2);
        frame->stack->push(val1);
    }

    void Opc::f2d(Frame* frame) {
        SE8::Value* value = frame->stack->pop();
        if (value->type == SE8::Float) {
            float val = *(uint32_t*)((uint64_t) value + 1);
            frame->stack->pushDouble(static_cast<double>(val));
        }
    }

    void Opc::f2i(Frame* frame) {
        SE8::Value* value = frame->stack->pop();
        if (value->type == SE8::Float) {
            float val = *(uint32_t*)((uint64_t) value + 1);
            frame->stack->pushInt(static_cast<int32_t>(val));
        }
    }

    void Opc::f2l(Frame* frame) {
        SE8::Value* value = frame->stack->pop();
        if (value->type == SE8::Float) {
            float val = *(uint32_t*)((uint64_t) value + 1);
            frame->stack->pushLong(static_cast<int64_t>(val));
        }
    }

    void Opc::fadd(Frame* frame) {
        SE8::Value* value2 = frame->stack->pop();
        SE8::Value* value1 = frame->stack->pop();
        if (value1->type == SE8::Float && value2->type == SE8::Float) {
            float val1 = *(uint32_t*)((uint64_t) value1 + 1);
            float val2 = *(uint32_t*)((uint64_t) value2 + 1);
            frame->stack->pushFloat(val1 + val2);
        }
    }

    void Opc::faload(Frame* frame) {

    }

    void Opc::fastore(Frame* frame) {

    }

    void Opc::fcmpg(Frame* frame) {
        SE8::Value* value2 = frame->stack->pop();
        SE8::Value* value1 = frame->stack->pop();
        if (value1->type == SE8::NaN || value2->type == SE8::NaN) {
            frame->stack->pushInt(1);
        } else if (value1->type == SE8::Float && value2->type == SE8::Float) {
            float val1 = *(uint32_t*)((uint64_t) value1 + 1);
            float val2 = *(uint32_t*)((uint64_t) value2 + 1);
            if (val1 == val2) {
                frame->stack->pushInt(0);
            } else if (val1 > val2) {
                frame->stack->pushInt(1);
            } else if (val1 < val2) {
                frame->stack->pushInt(-1);
            }
        }
    }

    void Opc::fcpml(Frame* frame) {
        SE8::Value* value2 = frame->stack->pop();
        SE8::Value* value1 = frame->stack->pop();
        if (value1->type == SE8::NaN || value2->type == SE8::NaN) {
            frame->stack->pushInt(-1);
        } else if (value1->type == SE8::Float && value2->type == SE8::Float) {
            float val1 = *(uint32_t*)((uint64_t) value1 + 1);
            float val2 = *(uint32_t*)((uint64_t) value2 + 1);
            if (val1 == val2) {
                frame->stack->pushInt(0);
            } else if (val1 > val2) {
                frame->stack->pushInt(1);
            } else if (val1 < val2) {
                frame->stack->pushInt(-1);
            }
        }
    }

    void Opc::fconst_0(Frame* frame) {
        frame->stack->pushFloat(0.0);
    }

    void Opc::fconst_1(Frame* frame) {
        frame->stack->pushFloat(1.0);
    }

    void Opc::fconst_2(Frame* frame) {
        frame->stack->pushFloat(2.0);
    }

    void Opc::fdiv(Frame* frame) {
        SE8::Value* value2 = frame->stack->pop();
        SE8::Value* value1 = frame->stack->pop();
        if (value1->type == SE8::Float && value2->type == SE8::Float) {
            float val2 = *(uint32_t*)((uint64_t) value2 + 1);
            if (val2 == 0) {
                frame->stack->pushNaN();
            } else {
                float val1 = *(uint32_t*)((uint64_t) value1 + 1);
                frame->stack->pushFloat(val1 / val2);
            }
        }
    }

    void Opc::fload(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(frame->widened ? frame->reader->u2B() : frame->reader->u1());
        uint8_t type = frame->locals->getType(ptr);
        if (type == SE8::Float) {
            frame->stack->pushFloat(frame->locals->get32(ptr));
        } else if (type == SE8::NaN) {
            frame->stack->pushNaN();
        } else if (type == SE8::Null) {
            frame->stack->pushNull();
        }
        frame->widened = false;
    }

    void Opc::fload_0(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(0);
        uint8_t type = frame->locals->getType(ptr);
        if (type == SE8::Float) {
            frame->stack->pushFloat(frame->locals->get32(ptr));
        } else if (type == SE8::NaN) {
            frame->stack->pushNaN();
        } else if (type == SE8::Null) {
            frame->stack->pushNull();
        }
        frame->widened = false;
    }

    void Opc::fload_1(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(1);
        uint8_t type = frame->locals->getType(ptr);
        if (type == SE8::Float) {
            frame->stack->pushFloat(frame->locals->get32(ptr));
        } else if (type == SE8::NaN) {
            frame->stack->pushNaN();
        } else if (type == SE8::Null) {
            frame->stack->pushNull();
        }
        frame->widened = false;
    }

    void Opc::fload_2(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(2);
        uint8_t type = frame->locals->getType(ptr);
        if (type == SE8::Float) {
            frame->stack->pushFloat(frame->locals->get32(ptr));
        } else if (type == SE8::NaN) {
            frame->stack->pushNaN();
        } else if (type == SE8::Null) {
            frame->stack->pushNull();
        }
        frame->widened = false;
    }

    void Opc::fload_3(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(3);
        uint8_t type = frame->locals->getType(ptr);
        if (type == SE8::Float) {
            frame->stack->pushFloat(frame->locals->get32(ptr));
        } else if (type == SE8::NaN) {
            frame->stack->pushNaN();
        } else if (type == SE8::Null) {
            frame->stack->pushNull();
        }
        frame->widened = false;
    }

    void Opc::fmul(Frame* frame) {
        SE8::Value* value2 = frame->stack->pop();
        SE8::Value* value1 = frame->stack->pop();
        if (value1->type == SE8::Float && value2->type == SE8::Float) {
            float val1 = *(uint32_t*)((uint64_t) value1 + 1);
            float val2 = *(uint32_t*)((uint64_t) value2 + 1);
            frame->stack->pushFloat(val1 * val2);
        }
    }

    void Opc::fneg(Frame* frame) {
        SE8::Value* value = frame->stack->pop();
        if (value->type == SE8::Float) {
            float val = *(uint32_t*)((uint64_t) value + 1);
            frame->stack->pushFloat(-val);
        }
    }

    void Opc::frem(Frame* frame) {

    }

    void Opc::freturn(Frame* frame) {

    }

    void Opc::fstore(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(frame->widened ? frame->reader->u2B() : frame->reader->u1());
        SE8::Value* value = frame->stack->pop();
        frame->locals->setType(ptr, value->type);
        if (value->type == SE8::Float) {
            frame->locals->set32(ptr, *(uint32_t*)(&value->bytes));
        }
        frame->widened = false;
    }

    void Opc::fstore_0(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(0);
        SE8::Value* value = frame->stack->pop();
        frame->locals->setType(ptr, value->type);
        if (value->type == SE8::Float) {
            frame->locals->set32(ptr, *(uint32_t*)(&value->bytes));
        }
        frame->widened = false;
    }

    void Opc::fstore_1(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(1);
        SE8::Value* value = frame->stack->pop();
        frame->locals->setType(ptr, value->type);
        if (value->type == SE8::Float) {
            frame->locals->set32(ptr, *(uint32_t*)(&value->bytes));
        }
        frame->widened = false;
    }

    void Opc::fstore_2(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(2);
        SE8::Value* value = frame->stack->pop();
        frame->locals->setType(ptr, value->type);
        if (value->type == SE8::Float) {
            frame->locals->set32(ptr, *(uint32_t*)(&value->bytes));
        }
        frame->widened = false;
    }

    void Opc::fstore_3(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(3);
        SE8::Value* value = frame->stack->pop();
        frame->locals->setType(ptr, value->type);
        if (value->type == SE8::Float) {
            frame->locals->set32(ptr, *(uint32_t*)(&value->bytes));
        }
        frame->widened = false;
    }

    void Opc::fsub(Frame* frame) {
        SE8::Value* value2 = frame->stack->pop();
        SE8::Value* value1 = frame->stack->pop();
        if (value1->type == SE8::Float && value2->type == SE8::Float) {
            float val1 = *(uint32_t*)((uint64_t) value1 + 1);
            float val2 = *(uint32_t*)((uint64_t) value2 + 1);
            frame->stack->pushFloat(val1 - val2);
        }
    }

    void Opc::getfield(Frame* frame) {

    }

    void Opc::getstatic(Frame* frame) {

    }

    void Opc::goto_(Frame* frame) {
        frame->reader->index += frame->reader->u2B();
    }

    void Opc::goto_w(Frame* frame) {
        frame->reader->index += frame->reader->u4B();
    }

    void Opc::i2b(Frame* frame) {
        int32_t val = *(uint32_t*)((uint64_t) frame->stack->pop() + 1);
        frame->stack->pushByte(static_cast<uint8_t>(val));
    }

    void Opc::i2c(Frame* frame) {
        int32_t val = *(uint32_t*)((uint64_t) frame->stack->pop() + 1);
        frame->stack->pushChar(static_cast<uint16_t>(val));
    }

    void Opc::i2d(Frame* frame) {
        int32_t val = *(uint32_t*)((uint64_t) frame->stack->pop() + 1);
        frame->stack->pushDouble(static_cast<double>(val));
    }

    void Opc::i2f(Frame* frame) {
        int32_t val = *(uint32_t*)((uint64_t) frame->stack->pop() + 1);
        frame->stack->pushFloat(static_cast<float>(val));
    }

    void Opc::i2l(Frame* frame) {
        int32_t val = *(uint32_t*)((uint64_t) frame->stack->pop() + 1);
        frame->stack->pushLong(static_cast<int64_t>(val));
    }

    void Opc::i2s(Frame* frame) {
        int32_t val = *(uint32_t*)((uint64_t) frame->stack->pop() + 1);
        frame->stack->pushShort(static_cast<int16_t>(val));
    }

    void Opc::iadd(Frame* frame) {
        SE8::Value* value2 = frame->stack->pop();
        SE8::Value* value1 = frame->stack->pop();
        if (value1->type == SE8::Int && value2->type == SE8::Int) {
            int32_t val1 = *(uint32_t*)((uint64_t) value1 + 1);
            int32_t val2 = *(uint32_t*)((uint64_t) value2 + 1);
            frame->stack->pushInt(val1 + val2);
        }
    }

    void Opc::iaload(Frame* frame) {

    }

    void Opc::iand(Frame* frame) {

    }

    void Opc::iastore(Frame* frame) {

    }

    void Opc::iconst_m1(Frame* frame) {
        frame->stack->pushInt(-1);
    }

    void Opc::iconst_0(Frame* frame) {
        frame->stack->pushInt(0);
    }

    void Opc::iconst_1(Frame* frame) {
        frame->stack->pushInt(1);
    }

    void Opc::iconst_2(Frame* frame) {
        frame->stack->pushInt(2);
    }

    void Opc::iconst_3(Frame* frame) {
        frame->stack->pushInt(3);
    }

    void Opc::iconst_4(Frame* frame) {
        frame->stack->pushInt(4);
    }

    void Opc::iconst_5(Frame* frame) {
        frame->stack->pushInt(5);
    }

    void Opc::idiv(Frame* frame) {
        SE8::Value* value2 = frame->stack->pop();
        SE8::Value* value1 = frame->stack->pop();
        if (value1->type == SE8::Int && value2->type == SE8::Int) {
            int val2 = *(uint32_t*)((uint64_t) value2 + 1);
            if (val2 == 0) {
                frame->stack->pushNaN();
            } else {
                int val1 = *(uint32_t*)((uint64_t) value1 + 1);
                frame->stack->pushInt(val1 / val2);
            }
        }
    }

    void Opc::if_acmpeq(Frame* frame) {

    }

    void Opc::if_acmpne(Frame* frame) {

    }

    void Opc::if_icmpeq(Frame* frame) {

    }

    void Opc::if_icmpne(Frame* frame) {

    }

    void Opc::if_icmplt(Frame* frame) {

    }

    void Opc::if_icmpge(Frame* frame) {

    }

    void Opc::if_icmpgt(Frame* frame) {

    }

    void Opc::if_icmple(Frame* frame) {

    }

    void Opc::ifeq(Frame* frame) {

    }

    void Opc::ifne(Frame* frame) {

    }

    void Opc::iflt(Frame* frame) {

    }

    void Opc::ifge(Frame* frame) {

    }

    void Opc::ifgt(Frame* frame) {

    }

    void Opc::ifle(Frame* frame) {

    }

    void Opc::ifnonnull(Frame* frame) {

    }

    void Opc::ifnull(Frame* frame) {

    }

    void Opc::iinc(Frame* frame) {

    }

    void Opc::iload(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(frame->widened ? frame->reader->u2B() : frame->reader->u1());
        uint8_t type = frame->locals->getType(ptr);
        if (type == SE8::Float) {
            frame->stack->pushInt(frame->locals->get32(ptr));
        } else if (type == SE8::NaN) {
            frame->stack->pushNaN();
        } else if (type == SE8::Null) {
            frame->stack->pushNull();
        }
        frame->widened = false;
    }

    void Opc::iload_0(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(0);
        uint8_t type = frame->locals->getType(ptr);
        if (type == SE8::Float) {
            frame->stack->pushInt(frame->locals->get32(ptr));
        } else if (type == SE8::NaN) {
            frame->stack->pushNaN();
        } else if (type == SE8::Null) {
            frame->stack->pushNull();
        } else {
            // throw type exception
        }
        frame->widened = false;
    }

    void Opc::iload_1(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(1);
        uint8_t type = frame->locals->getType(ptr);
        if (type == SE8::Float) {
            frame->stack->pushInt(frame->locals->get32(ptr));
        } else if (type == SE8::NaN) {
            frame->stack->pushNaN();
        } else if (type == SE8::Null) {
            frame->stack->pushNull();
        }
        frame->widened = false;
    }

    void Opc::iload_2(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(2);
        uint8_t type = frame->locals->getType(ptr);
        if (type == SE8::Float) {
            frame->stack->pushInt(frame->locals->get32(ptr));
        } else if (type == SE8::NaN) {
            frame->stack->pushNaN();
        } else if (type == SE8::Null) {
            frame->stack->pushNull();
        }
        frame->widened = false;
    }

    void Opc::iload_3(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(3);
        uint8_t type = frame->locals->getType(ptr);
        if (type == SE8::Float) {
            frame->stack->pushInt(frame->locals->get32(ptr));
        } else if (type == SE8::NaN) {
            frame->stack->pushNaN();
        } else if (type == SE8::Null) {
            frame->stack->pushNull();
        }
        frame->widened = false;
    }

    void Opc::imul(Frame* frame) {
        SE8::Value* value2 = frame->stack->pop();
        SE8::Value* value1 = frame->stack->pop();
        if (value1->type == SE8::Int && value2->type == SE8::Int) {
            int32_t val1 = *(uint32_t*)((uint64_t) value1 + 1);
            int32_t val2 = *(uint32_t*)((uint64_t) value2 + 1);
            frame->stack->pushInt(val1 * val2);
        }
    }

    void Opc::ineg(Frame* frame) {
        int32_t val1 = *(uint32_t*)((uint64_t) frame->stack->pop() + 1);
        frame->stack->pushInt(-val1);
    }

    void Opc::instanceof(Frame* frame) {

    }

    void Opc::invokedynamic(Frame* frame) {

    }

    void Opc::invokeinterface(Frame* frame) {

    }

    void Opc::invokespecial(Frame* frame) {

    }

    void Opc::invokestatic(Frame* frame) {

    }

    void Opc::invokevirtual(Frame* frame) {

    }

    void Opc::ior(Frame* frame) {

    }

    void Opc::irem(Frame* frame) {

    }

    void Opc::ireturn(Frame* frame) {

    }

    void Opc::ishl(Frame* frame) {
        SE8::Value* value2 = frame->stack->pop();
        SE8::Value* value1 = frame->stack->pop();
        if (value1->type == SE8::Int && value2->type == SE8::Int) {
            int32_t val1 = *(uint32_t*)((uint64_t) value1 + 1);
            int64_t val2 = *(uint64_t*)((uint64_t) value2 + 1);
            frame->stack->pushInt(val1 << val2);
        } else if (value1->type == SE8::NaN || value2->type == SE8::NaN) {
            frame->stack->pushNaN();
        } else if (value1->type == SE8::Null || value2->type == SE8::Null) {
            frame->stack->pushNull();
        }
    }

    void Opc::ishr(Frame* frame) {
        SE8::Value* value2 = frame->stack->pop();
        SE8::Value* value1 = frame->stack->pop();
        if (value1->type == SE8::Int && value2->type == SE8::Int) {
            int32_t val1 = *(uint32_t*)((uint64_t) value1 + 1);
            int64_t val2 = *(uint64_t*)((uint64_t) value2 + 1);
            frame->stack->pushInt(val1 >> val2);
        } else if (value1->type == SE8::NaN || value2->type == SE8::NaN) {
            frame->stack->pushNaN();
        } else if (value1->type == SE8::Null || value2->type == SE8::Null) {
            frame->stack->pushNull();
        }
    }

    void Opc::istore(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(frame->widened ? frame->reader->u2B() : frame->reader->u1());
        SE8::Value* value = frame->stack->pop();
        frame->locals->setType(ptr, value->type);
        if (value->type == SE8::Int) {
            frame->locals->set32(ptr, *(uint32_t*)(&value->bytes));
        }
        frame->widened = false;
    }

    void Opc::istore_0(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(0);
        SE8::Value* value = frame->stack->pop();
        frame->locals->setType(ptr, value->type);
        if (value->type == SE8::Int) {
            frame->locals->set32(ptr, *(uint32_t*)(&value->bytes));
        }
        frame->widened = false;
    }

    void Opc::istore_1(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(1);
        SE8::Value* value = frame->stack->pop();
        frame->locals->setType(ptr, value->type);
        if (value->type == SE8::Int) {
            frame->locals->set32(ptr, *(uint32_t*)(&value->bytes));
        }
        frame->widened = false;
    }

    void Opc::istore_2(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(2);
        SE8::Value* value = frame->stack->pop();
        frame->locals->setType(ptr, value->type);
        if (value->type == SE8::Int) {
            frame->locals->set32(ptr, *(uint32_t*)(&value->bytes));
        }
        frame->widened = false;
    }

    void Opc::istore_3(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(3);
        SE8::Value* value = frame->stack->pop();
        frame->locals->setType(ptr, value->type);
        if (value->type == SE8::Int) {
            frame->locals->set32(ptr, *(uint32_t*)(&value->bytes));
        }
        frame->widened = false;
    }

    void Opc::isub(Frame* frame) {
        SE8::Value* value2 = frame->stack->pop();
        SE8::Value* value1 = frame->stack->pop();
        if (value1->type == SE8::Int && value2->type == SE8::Int) {
            int32_t val1 = *(uint32_t*)((uint64_t) value1 + 1);
            int32_t val2 = *(uint32_t*)((uint64_t) value2 + 1);
            frame->stack->pushInt(val1 - val2);
        }
    }

    void Opc::iushr(Frame* frame) {
        SE8::Value* value2 = frame->stack->pop();
        SE8::Value* value1 = frame->stack->pop();
        if (value1->type == SE8::Int && value2->type == SE8::Int) {
            int32_t val1 = *(uint32_t*)((uint64_t) value1 + 1);
            int64_t val2 = *(uint64_t*)((uint64_t) value2 + 1);
            int32_t mask = 0x7fffffff;
            mask = mask >> val2;
            mask = (mask << 1) | 1;
            frame->stack->pushLong((val1 >> val2) & mask);
        } else if (value1->type == SE8::NaN || value2->type == SE8::NaN) {
            frame->stack->pushNaN();
        } else if (value1->type == SE8::Null || value2->type == SE8::Null) {
            frame->stack->pushNull();
        }
    }

    void Opc::ixor(Frame* frame) {

    }

    void Opc::jsr(Frame* frame) {
        uint16_t jmp = frame->reader->u2B();
        frame->stack->pushInt(frame->reader->index);
        frame->reader->index += jmp;
    }

    void Opc::jsr_w(Frame* frame) {
        uint32_t jmp = frame->reader->u4B();
        frame->stack->pushInt(frame->reader->index);
        frame->reader->index += jmp;
    }

    void Opc::l2d(Frame* frame) {

    }

    void Opc::l2f(Frame* frame) {

    }

    void Opc::l2i(Frame* frame) {

    }

    void Opc::ladd(Frame* frame) {

    }

    void Opc::laload(Frame* frame) {

    }

    void Opc::land(Frame* frame) {

    }

    void Opc::lastore(Frame* frame) {

    }

    void Opc::lcmp(Frame* frame) {

    }

    void Opc::lconst_0(Frame* frame) {
        frame->stack->pushLong(0);
    }

    void Opc::lconst_1(Frame* frame) {
        frame->stack->pushLong(1);
    }

    void Opc::ldc(Frame* frame) {

    }

    void Opc::ldc_w(Frame* frame) {

    }

    void Opc::ldc2_w(Frame* frame) {

    }

    void Opc::ldiv(Frame* frame) {
        SE8::Value* value2 = frame->stack->pop();
        SE8::Value* value1 = frame->stack->pop();
        if (value1->type == SE8::Long && value2->type == SE8::Long) {
            int64_t val2 = *(uint64_t*)((uint64_t) value2 + 1);
            if (val2 == 0) {
                frame->stack->pushNaN();
            } else {
                int64_t val1 = *(uint64_t*)((uint64_t) value1 + 1);
                frame->stack->pushLong(val1 / val2);
            }
        }
    }

    void Opc::lload(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(frame->widened ? frame->reader->u2B() : frame->reader->u1());
        uint8_t type = frame->locals->getType(ptr);
        if (type == SE8::Long) {
            frame->stack->pushLong(frame->locals->get64(ptr));
        } else if (type == SE8::NaN) {
            frame->stack->pushNaN();
        } else if (type == SE8::Null) {
            frame->stack->pushNull();
        }
        frame->widened = false;
    }

    void Opc::lload_0(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(0);
        uint8_t type = frame->locals->getType(ptr);
        if (type == SE8::Long) {
            frame->stack->pushLong(frame->locals->get64(ptr));
        } else if (type == SE8::NaN) {
            frame->stack->pushNaN();
        } else if (type == SE8::Null) {
            frame->stack->pushNull();
        }
        frame->widened = false;
    }

    void Opc::lload_1(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(1);
        uint8_t type = frame->locals->getType(ptr);
        if (type == SE8::Long) {
            frame->stack->pushLong(frame->locals->get64(ptr));
        } else if (type == SE8::NaN) {
            frame->stack->pushNaN();
        } else if (type == SE8::Null) {
            frame->stack->pushNull();
        }
        frame->widened = false;
    }

    void Opc::lload_2(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(2);
        uint8_t type = frame->locals->getType(ptr);
        if (type == SE8::Long) {
            frame->stack->pushLong(frame->locals->get64(ptr));
        } else if (type == SE8::NaN) {
            frame->stack->pushNaN();
        } else if (type == SE8::Null) {
            frame->stack->pushNull();
        }
        frame->widened = false;
    }

    void Opc::lload_3(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(3);
        uint8_t type = frame->locals->getType(ptr);
        if (type == SE8::Long) {
            frame->stack->pushLong(frame->locals->get64(ptr));
        } else if (type == SE8::NaN) {
            frame->stack->pushNaN();
        } else if (type == SE8::Null) {
            frame->stack->pushNull();
        }
        frame->widened = false;
    }

    void Opc::lmul(Frame* frame) {
        SE8::Value* value2 = frame->stack->pop();
        SE8::Value* value1 = frame->stack->pop();
        if (value1->type == SE8::Long && value2->type == SE8::Long) {
            int64_t val1 = *(uint64_t*)((uint64_t) value1 + 1);
            int64_t val2 = *(uint64_t*)((uint64_t) value2 + 1);
            frame->stack->pushLong(val1 * val2);
        }
    }

    void Opc::lneg(Frame* frame) {
        int64_t val = *(uint64_t*)((uint64_t) frame->stack->pop() + 1);
        frame->stack->pushLong(-val);
    }

    void Opc::lookupswitch(Frame* frame) {

    }

    void Opc::lor(Frame* frame) {

    }

    void Opc::lrem(Frame* frame) {

    }

    void Opc::lreturn(Frame* frame) {

    }

    void Opc::lshl(Frame* frame) {
        SE8::Value* value2 = frame->stack->pop();
        SE8::Value* value1 = frame->stack->pop();
        if (value1->type == SE8::Long && value2->type == SE8::Int) {
            int32_t val1 = *(uint32_t*)((uint64_t) value1 + 1);
            int64_t val2 = *(uint64_t*)((uint64_t) value2 + 1);
            frame->stack->pushLong(val1 << val2);
        } else if (value1->type == SE8::NaN || value2->type == SE8::NaN) {
            frame->stack->pushNaN();
        } else if (value1->type == SE8::Null || value2->type == SE8::Null) {
            frame->stack->pushNull();
        }
    }

    void Opc::lshr(Frame* frame) {
        SE8::Value* value2 = frame->stack->pop();
        SE8::Value* value1 = frame->stack->pop();
        if (value1->type == SE8::Long && value2->type == SE8::Int) {
            int32_t val1 = *(uint32_t*)((uint64_t) value1 + 1);
            int64_t val2 = *(uint64_t*)((uint64_t) value2 + 1);
            frame->stack->pushLong(val1 >> val2);
        } else if (value1->type == SE8::NaN || value2->type == SE8::NaN) {
            frame->stack->pushNaN();
        } else if (value1->type == SE8::Null || value2->type == SE8::Null) {
            frame->stack->pushNull();
        }
    }

    void Opc::lstore(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(frame->widened ? frame->reader->u2B() : frame->reader->u1());
        SE8::Value* value = frame->stack->pop();
        frame->locals->setType(ptr, value->type);
        if (value->type == SE8::Long) {
            frame->locals->set64(ptr, *(uint64_t*)(&value->bytes));
        }
        frame->widened = false;
    }

    void Opc::lstore_0(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(0);
        SE8::Value* value = frame->stack->pop();
        frame->locals->setType(ptr, value->type);
        if (value->type == SE8::Long) {
            frame->locals->set64(ptr, *(uint64_t*)(&value->bytes));
        }
        frame->widened = false;
    }

    void Opc::lstore_1(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(1);
        SE8::Value* value = frame->stack->pop();
        frame->locals->setType(ptr, value->type);
        if (value->type == SE8::Long) {
            frame->locals->set64(ptr, *(uint64_t*)(&value->bytes));
        }
        frame->widened = false;
    }

    void Opc::lstore_2(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(2);
        SE8::Value* value = frame->stack->pop();
        frame->locals->setType(ptr, value->type);
        if (value->type == SE8::Long) {
            frame->locals->set64(ptr, *(uint64_t*)(&value->bytes));
        }
        frame->widened = false;
    }

    void Opc::lstore_3(Frame* frame) {
        uint64_t ptr = frame->locals->getPtr(3);
        SE8::Value* value = frame->stack->pop();
        frame->locals->setType(ptr, value->type);
        if (value->type == SE8::Long) {
            frame->locals->set64(ptr, *(uint64_t*)(&value->bytes));
        }
        frame->widened = false;
    }

    void Opc::lsub(Frame* frame) {
        SE8::Value* value2 = frame->stack->pop();
        SE8::Value* value1 = frame->stack->pop();
        if (value1->type == SE8::Long && value2->type == SE8::Long) {
            int64_t val1 = *(uint64_t*)((uint64_t) value1 + 1);
            int64_t val2 = *(uint64_t*)((uint64_t) value2 + 1);
            frame->stack->pushDouble(val1 - val2);
        }
    }

    void Opc::lushr(Frame* frame) {
        SE8::Value* value2 = frame->stack->pop();
        SE8::Value* value1 = frame->stack->pop();
        if (value1->type == SE8::Long && value2->type == SE8::Int) {
            int64_t val1 = *(uint64_t*)((uint64_t) value1 + 1);
            int64_t val2 = *(uint64_t*)((uint64_t) value2 + 1);
            int64_t mask = 0x7fffffffffffffff;
            mask = mask >> val2;
            mask = (mask << 1) | 1;
            frame->stack->pushLong((val1 >> val2) & mask);
        } else if (value1->type == SE8::NaN || value2->type == SE8::NaN) {
            frame->stack->pushNaN();
        } else if (value1->type == SE8::Null || value2->type == SE8::Null) {
            frame->stack->pushNull();
        } else {
            // type exception
        }
    }

    void Opc::lxor(Frame* frame) {

    }

    void Opc::monitorenter(Frame* frame) {

    }

    void Opc::monitorexit(Frame* frame) {

    }

    void Opc::multianewarray(Frame* frame) {

    }

    void Opc::new_(Frame* frame) {

    }

    void Opc::newarray(Frame* frame) {
        uint8_t arrayType = frame->reader->u1();
        uint32_t count = *(uint32_t*)((uint64_t) frame->stack->pop() + 1);
        if (count < 0) {
            // throw NegativeArraySizeException
        } else {
            // todo : should use garbage collection to allocate array
            uint8_t size = 1+getTypeSize(arrayType)*count;
            SE8::Array* arr = (SE8::Array*) malloc(1+4+size);
            arr->type = arrayType;
            arr->count = count;
            memset((uintptr_t)((uint64_t) arr + 5), 0x00, size);
            frame->stack->pushArrayRef((uint64_t) arr);
        }
    }

    void Opc::nop(Frame* frame) {
        // do nothing
    }

    void Opc::pop(Frame* frame) {
        frame->stack->wpop();
    }

    void Opc::pop2(Frame* frame) {
        frame->stack->wpop();
    }

    void Opc::putfield(Frame* frame) {

    }

    void Opc::putstatic(Frame* frame) {

    }

    void Opc::ret(Frame* frame) {

    }

    void Opc::return_(Frame* frame) {

    }

    void Opc::saload(Frame* frame) {

    }

    void Opc::sastore(Frame* frame) {

    }

    void Opc::sipush(Frame* frame) {
        frame->stack->pushShort(frame->reader->u2B());
    }

    void Opc::swap(Frame* frame) {
        SE8::Value* val1 = frame->stack->pop();
        SE8::Value* val2 = frame->stack->pop();
        frame->stack->push(val1);
        frame->stack->push(val2);
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

    void Frame::run(Value* args, uint32_t args_length) {
        widened = false;
        reader->index = 0;
        if (args_length > 0) {
            for (uint16_t i = 0; i < args_length; i++) {
                // todo
            }
        }
        for (; reader->index < code_length; reader->index++) {
            oct[reader->u1()](this);
        }
    }

}
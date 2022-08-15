#pragma once

#include "../types.h"
#include "../jvm.h"

#include "../../utils/reader.h"

#include "stack.h"
#include "heap.h"

namespace SE8 {

    class JVM;

    class Frame {
    private:
        uint64_t pid = 0; // main thread
        uintptr_t constant_pool;
        Stack* stack;
        Locals* locals;
        JVM* jvm;
        bool widened = false;
    public:
        
        Reader* reader;

        Frame(JVM* jvm, uintptr_t constant_pool);
        
        void throwExc();

        // opcodes impl

        void aaload();
        void aastore();
        void aconst_null();
        void aload();
        void aload_0();
        void aload_1();
        void aload_2();
        void aload_3();
        void anewarray();
        void areturn();
        void arraylength();
        void astore();
        void astore_0();
        void astore_1();
        void astore_2();
        void astore_3();
        void athrow();
        void baload();
        void bastore();
        void bipush();
        void caload();
        void castore();
        void checkcast();
        void d2f();
        void d2i();
        void d2l();
        void dadd();
        void daload();
        void dastore();
        void dcmpg();
        void dcmpl();
        void dconst_0();
        void dconst_1();
        void ddiv();
        void dload();
        void dload_0();
        void dload_1();
        void dload_2();
        void dload_3();
        void dmul();
        void dneg();
        void drem();
        void dreturn();
        void dstore();
        void dstore_0();
        void dstore_1();
        void dstore_2();
        void dstore_3();
        void dsub();
        void dup();
        void dup_x1();
        void dup_x2();
        void dup2();
        void dup2_x1();
        void dup2_x2();
        void f2d();
        void f2i();
        void f2l();
        void fadd();
        void faload();
        void fastore();
        void fcmpg();
        void fcpml();
        void fconst_0();
        void fconst_1();
        void fconst_2();
        void fdiv();
        void fload();
        void fload_0();
        void fload_1();
        void fload_2();
        void fload_3();
        void fmul();
        void fneg();
        void frem();
        void freturn();
        void fstore();
        void fstore_0();
        void fstore_1();
        void fstore_2();
        void fstore_3();
        void fsub();
        void getfield();
        void getstatic();
        void goto_();
        void goto_w();
        void i2b();
        void i2c();
        void i2d();
        void i2f();
        void i2l();
        void i2s();
        void iadd();
        void iaload();
        void iand();
        void iastore();
        void iconst_m1();
        void iconst_0();
        void iconst_1();
        void iconst_2();
        void iconst_3();
        void iconst_4();
        void iconst_5();
        void idiv();
        void if_acmpeq();
        void if_acmpne();
        void if_icmpeq();
        void if_icmpne();
        void if_icmplt();
        void if_icmpge();
        void if_icmpgt();
        void if_icmple();
        void ifeq();
        void ifne();
        void iflt();
        void ifge();
        void ifgt();
        void ifle();
        void ifnonnull();
        void ifnull();
        void iinc();
        void iload();
        void iload_0();
        void iload_1();
        void iload_2();
        void iload_3();
        void imul();
        void ineg();
        void instanceof();
        void invokedynamic();
        void invokeinterface();
        void invokespecial();
        void invokestatic();
        void invokevirtual();
        void ior();
        void irem();
        void ireturn();
        void ishl();
        void ishr();
        void istore();
        void istore_0();
        void istore_1();
        void istore_2();
        void istore_3();
        void isub();
        void iushr();
        void ixor();
        void jsr();
        void jsr_w();
        void l2d();
        void l2f();
        void l2i();
        void ladd();
        void laload();
        void land();
        void lastore();
        void lcmp();
        void lconst_0();
        void lconst_1();
        void ldc();
        void ldc_w();
        void ldc2_w();
        void ldiv();
        void lload();
        void lload_0();
        void lload_1();
        void lload_2();
        void lload_3();
        void lmul();
        void lneg();
        void lookupswitch();
        void lor();
        void lrem();
        void lreturn();
        void lshl();
        void lshr();
        void lstore();
        void lstore_0();
        void lstore_1();
        void lstore_2();
        void lstore_3();
        void lsub();
        void lushr();
        void lxor();
        void monitorenter();
        void monitorexit();
        void multianewarray();
        void new_();
        void newarray();
        void nop();
        void pop();
        void pop2();
        void putfield();
        void putstatic();
        void ret();
        void return_();
        void saload();
        void sastore();
        void sipush();
        void swap();
        void tableswitch();
        void wide();

    };

}
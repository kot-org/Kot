#include "classes.h"

namespace SE8 {

    MethodInterface* createBinding(uint16_t args_length, uint32_t (*fn)(JavaVM* jvm, uint32_t object, uint32_t* args, uint16_t args_length)) {
        MethodInterface* interface = (MethodInterface*) malloc(sizeof(MethodInterface));
        interface->args_length = args_length;
        interface->type = 1;
        NativeMethod* mth = (NativeMethod*) malloc(sizeof(NativeMethod));
        mth->fn = fn;
        interface->method = mth;
        return interface;
    }

    uint32_t jips_println_str_fn(JavaVM* jvm, uint32_t object, uint32_t* args, uint16_t args_length) {
        vector_t* rs = jvm->getRefSys();
        Printlog((char*) jvm->getClasses()->getClass("java/lang/String")->getField64(vector_get(rs, args[0]), "<pointer>"));
        return NULL;
    }

    uint32_t jips_println_int_fn(JavaVM* jvm, uint32_t object, uint32_t* args, uint16_t args_length) {
        vector_t* rs = jvm->getRefSys();
        int32_t value = args[0];
        char* temp = (char*) malloc(asi(value, 10));
        Printlog(itoa(value, temp, 10));
        free(temp);
        return NULL;
    }

    uint32_t jlsb_init_fn(JavaVM* jvm, uint32_t object, uint32_t* args, uint16_t args_length) {
        vector_t* rs = jvm->getRefSys();
        ClassArea* jls = jvm->getClasses()->getClass("java/lang/String");
        ClassArea* jlsb = jvm->getClasses()->getClass("java/lang/StringBuilder");
        uintptr_t obj = jls->newObject();
        jls->setField64(obj, "<pointer>", NULL);
        jls->setField32(obj, "<length>", 0);
        jlsb->setField64(vector_get(rs, object), "<string>", (uint64_t) obj);
        return NULL;
    }

    uint32_t jlsb_append_str_fn(JavaVM* jvm, uint32_t object, uint32_t* args, uint16_t args_length) {
        vector_t* rs = jvm->getRefSys();
        ClassArea* jls = jvm->getClasses()->getClass("java/lang/String");
        ClassArea* jlsb = jvm->getClasses()->getClass("java/lang/StringBuilder");
        uintptr_t self = vector_get(rs, object);
        uintptr_t to = (uintptr_t) jlsb->getField64(self, "<string>");
        uintptr_t from = vector_get(rs, args[0]);
        int32_t toLength = jls->getField32(to, "<length>");
        int32_t fromLength = jls->getField32(from, "<length>");
        if (fromLength > 0) {
            if (toLength == 0) {
                jlsb->setField64(self, "<string>", (uint64_t) from);
            } else {
                uintptr_t newStr = jls->newObject();
                int32_t newLength = toLength + fromLength;
                uintptr_t newBuffer = malloc(newLength);
                memcpy(newBuffer, to, toLength);
                memcpy((uintptr_t)((uint64_t) newBuffer + toLength), from, fromLength);
                jls->setField64(newStr, "<pointer>", (uint64_t) newBuffer);
                jls->setField32(newStr, "<length>", newLength);
                jlsb->setField64(self, "<string>", (uint64_t) newStr);
            }
        }
        return object;
    }

    uint32_t jlsb_toString_fn(JavaVM* jvm, uint32_t object, uint32_t* args, uint16_t args_length) {
        vector_t* rs = jvm->getRefSys();
        ClassArea* jlsb = jvm->getClasses()->getClass("java/lang/StringBuilder");
        vector_push(rs, (uintptr_t) jlsb->getField64(vector_get(rs, object), "<string>"));
        return rs->length-1;
    }

    uint32_t jlsb_append_int_fn(JavaVM* jvm, uint32_t object, uint32_t* args, uint16_t args_length) {

        vector_t* rs = jvm->getRefSys();
        ClassArea* jls = jvm->getClasses()->getClass("java/lang/String");
        ClassArea* jlsb = jvm->getClasses()->getClass("java/lang/StringBuilder");

        uintptr_t self = vector_get(rs, object);
        uintptr_t to = (uintptr_t) jlsb->getField64(self, "<string>");
        int32_t toLength = jls->getField32(to, "<length>");

        int32_t from = args[0];
        int32_t fromLength = asi(from, 10);

        char* fromStr = (char*) malloc(fromLength);
        itoa(from, fromStr, 10);

        uintptr_t newStr = jls->newObject();
        
        if (toLength == 0) {
            jls->setField64(newStr, "<pointer>", (uint64_t) fromStr);
            jls->setField32(newStr, "<length>", fromLength);
        } else {
            int32_t newLength = toLength + fromLength;
            uintptr_t newBuffer = malloc(newLength);
            memcpy(newBuffer, to, toLength);
            memcpy((uintptr_t)((uint64_t) newBuffer + toLength), fromStr, fromLength);
            jls->setField64(newStr, "<pointer>", (uint64_t) newBuffer);
            jls->setField32(newStr, "<length>", newLength);
        }

        jlsb->setField64(self, "<string>", (uint64_t) newStr);

        return object;

    }

    Classes::Classes(JavaVM* jvm) {

        this->jvm = jvm;

        vector_t* rs = jvm->getRefSys();

        areas = map_create();

        ClassArea* java_lang_Object = new ClassArea(NULL, "java/lang/Object");
        map_set(areas, "java/lang/Object", java_lang_Object);
        java_lang_Object->clinit(jvm);

        ClassArea* java_io_OutputStream = new ClassArea(java_lang_Object, "java/io/OutputStream");
        map_set(areas, "java/io/OutputStream", java_io_OutputStream);
        java_io_OutputStream->clinit(jvm);

        ClassArea* java_io_FilterOutputStream = new ClassArea(java_io_OutputStream, "java/io/FilterOutputStream");
        map_set(areas, "java/io/FilterOutputStream", java_io_FilterOutputStream);
        java_io_OutputStream->clinit(jvm);

        ClassArea* java_io_PrintStream = new ClassArea(java_io_FilterOutputStream, "java/io/PrintStream");
        map_set(areas, "java/io/PrintStream", java_io_PrintStream);
        java_io_PrintStream->registerMethod("println", "(Ljava/lang/String;)V", createBinding(1, &jips_println_str_fn));
        java_io_PrintStream->registerMethod("println", "(I)V", createBinding(1, &jips_println_int_fn));
        java_io_PrintStream->clinit(jvm);

        ClassArea* java_lang_String = new ClassArea(java_lang_Object, "java/lang/String");
        map_set(areas, "java/lang/String", java_lang_String);
        java_lang_String->registerField("<pointer>", 8);
        java_lang_String->registerField("<length>", 4);
        java_lang_String->clinit(jvm);

        ClassArea* java_lang_StringBuilder = new ClassArea(java_lang_Object, "java/lang/StringBuilder");
        map_set(areas, "java/lang/StringBuilder", java_lang_StringBuilder);
        java_lang_StringBuilder->registerField("<string>", 8);
        java_lang_StringBuilder->registerMethod("<init>", "()V", createBinding(0, &jlsb_init_fn));
        java_lang_StringBuilder->registerMethod("append", "(Ljava/lang/String;)Ljava/lang/StringBuilder;", createBinding(1, &jlsb_append_str_fn));
        java_lang_StringBuilder->registerMethod("append", "(I)Ljava/lang/StringBuilder;", createBinding(1, &jlsb_append_int_fn));
        java_lang_StringBuilder->registerMethod("toString", "()Ljava/lang/String;", createBinding(0, &jlsb_toString_fn));
        java_lang_StringBuilder->clinit(jvm);

        ClassArea* java_lang_System = new ClassArea(java_lang_Object, "java/lang/System");
        map_set(areas, "java/lang/System", java_lang_System);
        java_lang_System->registerStaticField("out", 4);
        java_lang_System->clinit(jvm);
        vector_push(rs, java_io_PrintStream->newObject());
        java_lang_System->setStaticField32("out", rs->length-1);

    }

    void Classes::loadClassBytes(uintptr_t bytes) {
        ClassParser* cp = new ClassParser(bytes);
        ClassArea* cl = new ClassArea(cp);
        map_set(areas, cl->getName(), cl);
    }

    ClassArea* Classes::getClass(char* name) {
        return (ClassArea*) map_get(areas, name);
    }

    void Classes::clinit() {
        for (uint64_t i = 0; i < areas->length; i++) {
            ClassArea* cl = (ClassArea*) map_geti(areas, i);
            if (cl->getConstantPool() != NULL) { cl->clinit(jvm); }
        }
    }

}
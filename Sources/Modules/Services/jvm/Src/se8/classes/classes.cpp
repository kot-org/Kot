#include "classes.h"

namespace SE8 {

    MethodInterface* createBinding(uint16_t args_length, uintptr_t (*fn)(JavaVM* jvm, uintptr_t object, uint32_t* args, uint16_t args_length)) {
        MethodInterface* interface = (MethodInterface*) malloc(sizeof(MethodInterface));
        interface->args_length = args_length;
        interface->type = 1;
        NativeMethod* mth = (NativeMethod*) malloc(sizeof(NativeMethod));
        mth->fn = fn;
        interface->method = mth;
        return interface;
    }

    uintptr_t jips_println_fn(JavaVM* jvm, uintptr_t object, uint32_t* args, uint16_t args_length) {
        vector_t* rs = jvm->getRefSys();
        Printlog((char*) jvm->getClasses()->getClass("java/lang/String")->getField64(vector_get(rs, args[0]), "<pointer>"));
        return NULL;
    }

    uintptr_t jlsb_init_fn(JavaVM* jvm, uintptr_t object, uint32_t* args, uint16_t args_length) {
        // todo
        return NULL;
    }

    uintptr_t jlsb_append_fn(JavaVM* jvm, uintptr_t object, uint32_t* args, uint16_t args_length) {
        // todo
        // volontairement pas de return pour page fault
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
        java_io_PrintStream->registerMethod("println", "(Ljava/lang/String;)V", createBinding(1, &jips_println_fn));
        java_io_PrintStream->clinit(jvm);

        ClassArea* java_lang_String = new ClassArea(java_lang_Object, "java/lang/String");
        map_set(areas, "java/lang/String", java_lang_String);
        java_lang_String->registerField("<pointer>", 8);
        java_lang_String->registerField("<length>", 4);
        java_lang_String->clinit(jvm);

        ClassArea* java_lang_StringBuilder = new ClassArea(java_lang_Object, "java/lang/StringBuilder");
        map_set(areas, "java/lang/StringBuilder", java_lang_StringBuilder);
        java_lang_StringBuilder->registerMethod("<init>", "()V", createBinding(1, &jlsb_init_fn));
        java_lang_StringBuilder->registerMethod("append", "(Ljava/lang/String;)Ljava/lang/StringBuilder;", createBinding(1, &jlsb_append_fn));
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
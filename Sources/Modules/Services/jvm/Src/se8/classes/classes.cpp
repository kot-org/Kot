#include "classes.h"

namespace SE8 {

    Classes::Classes() {
        areas = map_create();
        //map_set(areas, "java/lang/System", new Class());
    }

    void Classes::loadClassBytes(uintptr_t bytes) {
        Class* cl = new Class(bytes);
        map_set(areas, cl->getClassName(), cl);
    }

    Class* Classes::getClass(char* name) {
        return (Class*) map_get(areas, name);
    }

    Method* Classes::getMethod(char* className, char* methodName, char* descriptor) {
        Class* cl = getClass(className);
        if (cl == NULL) { return NULL; }
        return cl->getMethod(methodName, descriptor);
    }

    Method* Classes::getStaticMethod(char* className, char* methodName, char* descriptor) {
        Class* cl = getClass(className);
        if (cl == NULL) { return NULL; }
        return cl->getStaticMethod(methodName, descriptor);
    }

    void Classes::clinit(JavaVM* jvm) {
        for (uint64_t i = 0; i < areas->length; i++) {
            Class* cl = (Class*) map_geti(areas, i);
            if (cl != NULL) {
                Method* method = cl->getStaticMethod("<clinit>", "()V");
                if (method != NULL) {
                    Frame* frame = (Frame*) malloc(sizeof(Frame));
                    frame->init(jvm, cl, method);
                    frame->run(NULL, 0);
                }
            }
        }
    }

    void Classes::setStaticField(char* className, char* fieldName, Value* value) {
        getClass(className)->setStaticField(fieldName, value);
    }

    Value* Classes::getStaticField(char* className, char* fieldName) {
        return getClass(className)->getStaticField(fieldName);
    }

}
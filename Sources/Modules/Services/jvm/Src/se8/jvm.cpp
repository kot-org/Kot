#include "jvm.h"

namespace SE8 {

    JavaVM::JavaVM() {
        ref_sys = vector_create();
        vector_push(ref_sys, NULL);
        this->classes = new Classes(this);
        this->threads = new Threads();
        if (isOpCodeTableNotInit()) {
            initOpCodeTable();
        }
    }

    Classes* JavaVM::getClasses() {
        return classes;
    }

    Threads* JavaVM::getThreads() {
        return threads;
    }

    vector_t* JavaVM::getRefSys() {
        return ref_sys;
    }

    void JavaVM::run(uint32_t* args, uint16_t args_length) {
        classes->clinit();
        Thread* thr = threads->append();
        thr->setName("main");
        thr->run(this, this->entryPoint, "main", "([Ljava/lang/String;)V", args, args_length);
    }

    void JavaVM::setEntryPoint(char* entryPoint) {
        this->entryPoint = entryPoint;
    }

}
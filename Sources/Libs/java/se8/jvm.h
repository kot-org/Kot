#pragma once

#ifndef __SE8_JVM_H__
#define __SE8_JVM_H__

#include "frame/frame.h"
#include "classes/classes.h"
#include "threads/threads.h"

namespace SE8 {

    class Classes;
    class Threads;

    typedef void (*print_native)(char* str);

    void __default_print(char* str);

    class JavaVM {
    private:
        Classes* classes;
        Threads* threads;
        kot_vector_t* ref_sys;
        char* entryPoint;
        print_native out = &__default_print;
    public:
        JavaVM();
        void run(uint32_t* args, uint16_t args_length);
        Classes* getClasses();
        Threads* getThreads();
        kot_vector_t* getRefSys();
        void setEntryPoint(char* entryPoint);
        void setOutput(print_native fn);
        print_native getOutput();
        void loadClassBytes(uintptr_t bytes);
    };

}

#endif
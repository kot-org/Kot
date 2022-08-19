#pragma once

#include "frame/frame.h"
#include "classes/classes.h"
#include "threads/threads.h"

namespace SE8 {

    class Classes;
    class Threads;

    class JavaVM {
    private:
        Classes* classes;
        Threads* threads;
        vector_t* ref_sys;
        char* entryPoint;
    public:
        JavaVM();
        void run(uint32_t* args, uint16_t args_length);
        Classes* getClasses();
        Threads* getThreads();
        vector_t* getRefSys();
        void setEntryPoint(char* entryPoint);
    };

}
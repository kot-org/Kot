#pragma once

#include "frame/frame.h"
#include "classes/classes.h"
#include "threads/threads.h"

namespace SE8 {

    class Classes;

    class JavaVM {
    private:
        Classes* classes;
        Threads* threads;
        char* entryPoint;
    public:
        JavaVM();
        void run(Value* args, uint32_t args_length);
        Classes* getClasses();
        Threads* getThreads();
        void setEntryPoint(char* entryPoint);
    };

}
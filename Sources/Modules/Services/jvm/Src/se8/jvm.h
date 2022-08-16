#pragma once

#include "frame/frame.h"
#include "classes/classes.h"
#include "threads/threads.h"

namespace SE8 {

    class Classes;

    class JVM {
    private:
        Classes* classes;
        Threads* threads;
        char* entryPoint;
    public:
        JVM();
        void run();
        Classes* getClasses();
        Threads* getThreads();
        void setEntryPoint(char* entryPoint);
    };

}
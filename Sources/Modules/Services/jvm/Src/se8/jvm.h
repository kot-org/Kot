#pragma once

#include "frame/frame.h"
#include "classes/classes.h"
#include "threads/threads.h"

namespace SE8 {

    class JVM {
    private:
        Classes* classes;
        Threads* threads;
    public:
        JVM();
        void run();
        Classes* getClasses();
        Threads* getThreads();
    };

}
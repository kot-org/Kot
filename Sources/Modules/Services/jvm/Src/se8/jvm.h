#pragma once

#include "frame/frame.h"
#include "classes/classes.h"
#include "threads/threads.h"

namespace SE8 {

    class JVM {
    private:

        Classes* classes;
        threads* threads;

        size_t stack_size = 512000; // 512m

    public:

        JVM();

        void setStackSize(size_t size);
        size_t getStackSize();

        void loadClassBytes(uintptr_t bytes);

        void run();
        
    };

}
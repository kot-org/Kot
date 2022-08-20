#pragma once

#include <kot/sys.h>
#include <kot/utils/vector.h>

#include "../types.h"

namespace SE8 {

    class Frame;

    class Thread {
    private:
        uint64_t pid;
        bool used = false;
        char* name;
        thread_t thr;
    public:
        Thread(uint64_t pid);
        void run(JavaVM* jvm, char* className, char* methodName, char* signature, uint32_t* args, uint16_t args_length);
        void setName(char* name);
        char* getName();
        uint64_t getId();
        bool isUsed();
    };

}
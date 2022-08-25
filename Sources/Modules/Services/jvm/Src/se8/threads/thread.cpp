#include "thread.h"

namespace SE8 {

    Thread::Thread(uint64_t pid) {
        this->pid = pid;
        setName("unamed");
        // process_t self;
        // Sys_GetProcessKey(&self);
        // Sys_Createthread(self, (uintptr_t)&threadMain, PriviledgeService, &this->thr);
    }   

    void Thread::run(JavaVM* jvm, char* className, char* methodName, char* signature, uint32_t* args, uint16_t args_length) {
        used = true;
        // todo: actual thread management
        jvm->getClasses()->getClass(className)->runStaticMethod(jvm, methodName, signature, args, args_length);
        used = false;
    }

    bool Thread::isUsed() {
        return used;
    }

    uint64_t Thread::getId() {
        return pid;
    }

    char* Thread::getName() {
        return name;
    }

    void Thread::setName(char* name) {
        this->name = name;
    } 

}
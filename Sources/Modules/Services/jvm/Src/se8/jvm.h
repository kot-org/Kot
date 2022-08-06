#pragma once

#include "frame/frame.h"
#include "opcodes/opcodes.h"
#include "class/class.h"

class JVM8 {
private:
    SE8Class* classes;
public:

    JVM8();

    void loadJarFile();
    void loadClassFile(uintptr_t bytes);
    
};
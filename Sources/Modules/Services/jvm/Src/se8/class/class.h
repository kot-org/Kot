#pragma once

#include "loader.h"

#include <kot/utils/map.h>

class SE8Class {
private:
    vector_t* areas;
public:
    SE8Class();
    void loadJarFile();
    void loadClassFile(uintptr_t buffer);
};
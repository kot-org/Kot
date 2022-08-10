#include "classes.h"

#include <kot/cstring.h>

namespace SE8 {

    Classes::Classes() {
    areas = map_create();
    }

    void Classes::loadClassBytes(uintptr_t bytes) {
        ClassLoader* classLoader = new ClassLoader(bytes);
        map_set(areas, 0/*classLoader->getName()*/, classLoader);
    }

}
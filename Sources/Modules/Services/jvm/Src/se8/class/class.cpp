#include "class.h"

#include <kot/cstring.h>

SE8Class::SE8Class() {
    areas = map_create();
}

void SE8Class::loadClassFile(uintptr_t buffer) {
    SE8ClassLoader* classLoader = new SE8ClassLoader(buffer);
    map_set(areas, classLoader->getName(), classLoader);
}

void SE8Class::loadJarFile() {
    
}
#include "classes.h"

namespace SE8 {

    Classes::Classes() {
        areas = map_create();
    }

    void Classes::loadClassBytes(uintptr_t bytes) {
        Class* cl = new Class(bytes);
        map_set(areas, (uint64_t) cl->getClassName(), cl);
    }

    void Classes::clinit() {
        
    }

}
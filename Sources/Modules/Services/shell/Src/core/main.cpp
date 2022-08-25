#include <core/main.h>

extern "C" int main() {

    process_t self = NULL;
    Sys_GetProcessKey(&self);

    uint64_t wid = orb::CreateWindow(300, 300);

}
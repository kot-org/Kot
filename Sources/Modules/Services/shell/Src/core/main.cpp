#include <core/main.h>

extern "C" int main() {

    process_t self = Sys_GetProcess();

    uint64_t wid = orb::CreateWindow(300, 300);

}
#pragma once

#include <core/main.h>

class E1000Controller {
    private:
    
    public:
        E1000Controller(srv_pci_bar_info_t* BarInfo);
        uintptr_t memoryBase;
};
#include <controller/controller.h>

E1000Controller::E1000Controller(srv_pci_bar_info_t* BarInfo) {
    memoryBase = MapPhysical(BarInfo->Address, BarInfo->Size);
}
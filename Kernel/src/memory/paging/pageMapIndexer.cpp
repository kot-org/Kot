#include <memory/paging/PageMapIndexer.h>

PageMapIndexer::PageMapIndexer(uint64_t virtualAddress){
    virtualAddress >>= 12;
    P_i = virtualAddress & 0x1ff;
    virtualAddress >>= 9;
    PT_i = virtualAddress & 0x1ff;
    virtualAddress >>= 9;
    PD_i = virtualAddress & 0x1ff;
    virtualAddress >>= 9;
    PDP_i = virtualAddress & 0x1ff;
}

uint64_t GetVirtualAddressMap(uint64_t PDP_i, uint64_t PD_i, uint64_t PT_i, uint64_t P_i){
    uint64_t virtualAddress = 0;
    virtualAddress |= P_i << 12;
    virtualAddress |= PT_i << 21;
    virtualAddress |= PD_i << 30;
    virtualAddress |= PDP_i << 39;
    return virtualAddress;
}
#include "PageMapIndexer.h"

PageMapIndexer::PageMapIndexer(uint64_t virtualAddress) {
    virtualAddress >>= 12;
    _p = virtualAddress & 0x1ff;
    virtualAddress >>= 9;
    _pt = virtualAddress & 0x1ff;
    virtualAddress >>= 9;
    _pd = virtualAddress & 0x1ff;
    virtualAddress >>= 9;
    _pdp = virtualAddress & 0x1ff;
}
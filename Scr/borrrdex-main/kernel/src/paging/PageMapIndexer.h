#pragma once

#include <stdint.h>

class PageMapIndexer {
public:
    PageMapIndexer(uint64_t virtualAddress);

    uint64_t GetPDP() const { return _pdp; }
    uint64_t GetPD() const { return _pd; }
    uint64_t GetPT() const { return _pt; }
    uint64_t GetP() const { return _p; }

private:
    uint64_t _pdp;
    uint64_t _pd;
    uint64_t _pt;
    uint64_t _p;
};
#include "Paging.h"

void PageDirectoryEntry::SetFlag(PT_Flag flag, bool enabled) {
    uint64_t bitSelector = 1ULL << flag;
    if(enabled) {
        _value |= bitSelector;
    } else {
        _value &= ~bitSelector;
    }
}

bool PageDirectoryEntry::GetFlag(PT_Flag flag) const {
    uint64_t bitSelector = 1ULL << flag;
    return _value & bitSelector;
}

uint64_t PageDirectoryEntry::GetAddress() const {
    return (_value & 0x000ffffffffff000) >> 12ULL;
}

void PageDirectoryEntry::SetAddress(uint64_t address) {
    address &= 0x000000ffffffffff;
    _value &= 0xfff0000000000fff;
    _value |= (address << 12);
}
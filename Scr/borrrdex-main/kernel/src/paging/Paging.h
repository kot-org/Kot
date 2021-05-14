#pragma once

#include <stdint.h>

enum PT_Flag {
    Present = 0,
    ReadWrite = 1,
    UserSuper = 2,
    WriteThrough = 3,
    CacheDisabled = 4,
    Accessed = 5,
    LargerPages = 7,
    Custom0 = 9,
    Custom1 = 10,
    Custom2 = 11,
    NX = 63
};

struct PageDirectoryEntry {
    void SetFlag(PT_Flag flag, bool enabled);
    bool GetFlag(PT_Flag flag) const;
    void SetAddress(uint64_t address);
    uint64_t GetAddress() const;

    private:
        uint64_t _value;
};

struct PageTable {
    PageDirectoryEntry entries[512];
} __attribute__((aligned(0x1000))) ;
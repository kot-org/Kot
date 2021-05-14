#pragma once

#include <stddef.h>
#include <stdint.h>

class Bitmap {
public:
    Bitmap() : Bitmap(0, NULL) {}
    Bitmap(size_t size, void* buffer);

    bool operator[](uint64_t index) const;
    bool Set(uint64_t index, bool value);
    bool SetNext(uint64_t* index);
    void Clear();

    size_t GetSize() const;
private:
    size_t _size;
    void* _buffer;
};
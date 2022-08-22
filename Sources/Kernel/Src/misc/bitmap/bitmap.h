#pragma once
#include <kot/types.h>
#include <lib/lib.h>

class Bitmap{
    public:
        size64_t Size;
        uint8_t* Buffer;
        bool operator[](uint64_t index);
        bool Get(uint64_t index);
        bool Set(uint64_t index, bool value);
};

class BitmapHeap{
    private:
        Bitmap* bitmap;
    public:
        BitmapHeap(size64_t size);
        void Free();
        bool operator[](uint64_t index);
        bool Get(uint64_t index);
        bool Set(uint64_t index, bool value);    
};
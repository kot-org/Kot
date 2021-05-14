#include "Bitmap.h"
#include "string.h"

Bitmap::Bitmap(size_t size, void* buffer)
    :_size(size)
    ,_buffer(buffer)
    {
    }

bool Bitmap::operator[](uint64_t index) const {
    if(index > _size * 8) {
        return false;
    }

    uint8_t* b = (uint8_t *)_buffer;
    uint64_t byteIndex = index / 8;
    uint8_t bitIndex = index % 8;

    return (b[byteIndex] >> bitIndex) & 0x1;
}

bool Bitmap::Set(uint64_t index, bool value) {
    if(index > _size * 8) {
        return false;
    }

    uint8_t* b = (uint8_t *)_buffer;
    uint64_t byteIndex = index / 8;
    uint8_t bitIndex = index % 8;

    if(value) {
        b[byteIndex] = b[byteIndex] | (1 << bitIndex);
    } else {
        b[byteIndex] = b[byteIndex] & ~(1 << bitIndex);
    }

    return true;
}

bool Bitmap::SetNext(uint64_t* index) {
    uint32_t* b = (uint32_t *)_buffer;
    for(uint32_t i = 0; i < _size / 4; i++) {
        if(b[i] != 0xffffffff) {
            uint32_t basepos = i*32;
            for(uint32_t j = 0; j < 32; j++) {
                if(basepos + j > _size) {
                    return false;
                }

                if(!operator[](basepos + j)) {
                    Set(basepos + j, true);
                    *index = basepos + j;
                    return true;
                }
            }

            return false;
        }
    }

    return false;
}

size_t Bitmap::GetSize() const {
    return _size;
}

void Bitmap::Clear() {
    memset(_buffer, 0, _size);
}
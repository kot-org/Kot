#include <misc/bitmap/bitmap.h>
#include <heap/heap.h>

bool Bitmap::operator[](uint64_t index){
    return Get(index);
}

bool Bitmap::Get(uint64_t index){
    if(index > Size * 8) return false;
    uint64_t byteIndex = index / 8;
    uint8_t bitIndex = index % 8;
    uint8_t bitIndexer = 0b10000000 >> bitIndex;
    return Buffer[byteIndex] & bitIndexer;
}

bool Bitmap::Set(uint64_t index, bool value){
    if (index > Size * 8) return false;
    uint64_t byteIndex = index / 8;
    uint8_t bitIndex = index % 8;
    uint8_t bitIndexer = 0b10000000 >> bitIndex;
    Buffer[byteIndex] &= ~bitIndexer;
    if (value){
        Buffer[byteIndex] |= bitIndexer;
    }
    return true;
}

bool Bitmap::GetAndSet(uint64_t index, bool value){
    if(index > Size * 8) return false;
    uint64_t byteIndex = index / 8;
    uint8_t bitIndex = index % 8;
    uint8_t bitIndexer = 0b10000000 >> bitIndex;
    bool Status = Buffer[byteIndex] & bitIndexer;
    Buffer[byteIndex] &= ~bitIndexer;
    if (value){
        Buffer[byteIndex] |= bitIndexer;
    }
    return Status;
}

BitmapHeap::BitmapHeap(size64_t size){
    bitmap = (Bitmap*)malloc(sizeof(Bitmap));
    bitmap->Size = size;
    bitmap->Buffer = (uint8_t*)malloc(DivideRoundUp(size, 8));
}

void BitmapHeap::Free(){
    free(bitmap->Buffer);
    free(bitmap);
}

bool BitmapHeap::operator[](uint64_t index){
    return bitmap->Get(index);
}

bool BitmapHeap::Get(uint64_t index){
    return bitmap->Get(index);
}

bool BitmapHeap::Set(uint64_t index, bool value){
    return bitmap->Set(index, value);
}


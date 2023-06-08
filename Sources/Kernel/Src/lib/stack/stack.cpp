#include <lib/stack/stack.h>

KStack* KStackInitialize(uint64_t sector_size) {
    KStack* Stack = (KStack*)kmalloc(sizeof(KStack));
    if (sector_size < 24) sector_size = 24;
    Stack->sector_size = sector_size + 8 + 1;
    Stack->current_sector = kmalloc(Stack->sector_size); // last sector address: 8; recovery byte: 1;
    Stack->top = 9;
    *(uint64_t*)(Stack->current_sector) = NULL;
    return Stack;
}

uint8_t KStack::pop8() {
    if (this->top < 9 + 1) {
        void* temp = (void*) *(uint64_t*)(current_sector);
        if (temp == NULL) { return NULL; }
        kfree(current_sector);
        current_sector = temp;
        top = sector_size - *(uint8_t*)((uint64_t) current_sector + 8);
        return pop16();
    } else {
        top --;
        return *(uint8_t*)((uint64_t) this->current_sector + this->top);
    }
}

uint16_t KStack::pop16() {
    if (this->top < 9 + 2) {
        void* temp = (void*) *(uint64_t*)(current_sector);
        if (temp == NULL) { return NULL; }
        kfree(current_sector);
        current_sector = temp;
        top = sector_size - *(uint8_t*)((uint64_t) current_sector + 8);
        return pop16();
    } else {
        top -= 2;
        return *(uint16_t*)((uint64_t) this->current_sector + this->top);
    }
}

uint32_t KStack::pop32() {
    if (this->top < 9 + 4) {
        void* temp = (void*) *(uint64_t*)(current_sector);
        if (temp == NULL) { return NULL; }
        kfree(current_sector);
        current_sector = temp;
        top = sector_size - *(uint8_t*)((uint64_t) current_sector + 8);
        return pop32();
    } else {
        top -= 4;
        return *(uint32_t*)((uint64_t) this->current_sector + this->top);
    }
}

uint64_t KStack::pop64() {
    if (this->top < 9 + 8) {
        void* temp = (void*) *(uint64_t*)(current_sector);
        if (temp == NULL) { return NULL; }
        kfree(current_sector);
        current_sector = temp;
        top = sector_size - *(uint8_t*)((uint64_t) current_sector + 8);
        return pop64();
    } else {
        top -= 8;
        return *(uint64_t*)((uint64_t) this->current_sector + this->top);
    }
}

void KStack::push8(uint8_t item) {
    if (this->top + 1 > this->sector_size) {
        *(uint8_t*)((uint64_t) current_sector + 8) = sector_size - top;
        void* temp = kmalloc(sector_size);
        *(uint64_t*)(temp) = (uint64_t) current_sector;
        current_sector = temp;
        top = 9;
        push16(item);
    } else {
        *(uint8_t*)((uint64_t) current_sector + top) = item;
        top++;
    }
}

void KStack::push16(uint16_t item) {
    if (this->top + 2 > this->sector_size) {
        *(uint8_t*)((uint64_t) current_sector + 8) = sector_size - top;
        void* temp = kmalloc(sector_size);
        *(uint64_t*)(temp) = (uint64_t) current_sector;
        current_sector = temp;
        top = 9;
        push16(item);
    } else {
        *(uint16_t*)((uint64_t) current_sector + top) = item;
        top += 2;
    }
}

void KStack::push32(uint32_t item) {
    if (this->top + 4 > this->sector_size) {
        *(uint8_t*)((uint64_t) current_sector + 8) = sector_size - top;
        void* temp = kmalloc(sector_size);
        *(uint64_t*)(temp) = (uint64_t) current_sector;
        current_sector = temp;
        top = 9;
        push32(item);
    } else {
        *(uint32_t*)((uint64_t) current_sector + top) = item;
        top += 4;
    }
}

void KStack::push64(uint64_t item) {
    if (this->top + 8 > this->sector_size) {
        *(uint8_t*)((uint64_t) current_sector + 8) = sector_size - top;
        void* temp = kmalloc(sector_size);
        *(uint64_t*)(temp) = (uint64_t) current_sector;
        current_sector = temp;
        top = 9;
        push64(item);
    } else {
        *(uint64_t*)((uint64_t) current_sector + top) = item;
        top += 8;
    }
}

void KStack::sinkInto(void* dest, uint64_t size) {
    uint64_t idx = (uint64_t) dest + size;
    while (idx != (uint64_t) dest) {
        idx--;
        *(uint8_t*)(idx) = pop8();
    }
}

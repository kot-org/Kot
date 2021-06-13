#include "IDT.h"

void IDTDescEntry::SetOffset(uint64_t offset){
    offset0 = (uint16_t)offset & 0xffff;
	offset1 = (uint16_t)(offset >> 16) & 0xffff;
	offset2 = (uint32_t)(offset >> 32);
}

uint64_t IDTDescEntry::GetOffset(){
	uint64_t offset = 0;
	offset |= (uint64_t) offset0;
	offset |= (uint64_t) offset1 << 16;
	offset |= (uint64_t) offset2 << 32;
    return offset;
}

void SetIDTGate(void* handler, uint8_t entryOffset, uint8_t type_attr, uint8_t selector, IDTR idtrl){
    IDTDescEntry* interrupt = (IDTDescEntry*)(idtrl.Offset + entryOffset * sizeof(IDTDescEntry));
    interrupt->SetOffset((uint64_t)handler);
    interrupt->type_attr = type_attr;
    interrupt->selector = selector;
}
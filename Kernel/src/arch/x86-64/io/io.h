#pragma once
#include "../../../lib/types.h"

void IoWrite8(uint16_t port, uint8_t data);
void IoWrite16(uint16_t port, uint16_t data);
void IoWrite32(uint16_t port, uint32_t data);

uint8_t IoRead8(uint16_t port);
uint16_t IoRead16(uint16_t port);
uint32_t IoRead32(uint16_t port);

void io_wait();

#pragma once
#include <kot/types.h>
#include <kot/x86_64.h>

bool IO_IN(uint8_t size, uint16_t port, uint32_t data);
uint32_t IO_OUT(uint8_t size, uint16_t port);

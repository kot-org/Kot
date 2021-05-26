#pragma once

#include <stdint.h>
#include "printf.h"
#include "../../graphics.h"
#include "../../memory/heap.h"


const char* to_string(uint64_t value);
const char* to_string(int64_t value);
const char* to_string(int value);
const char* to_hstring(uint64_t value);
const char* to_hstring(uint32_t value);
const char* to_hstring(uint16_t value);
const char* to_hstring(uint8_t value);
const char* to_string(double value, uint8_t decimalPlaces);
const char* to_string(double value);
const char* to_string(char c);

int strlen(char*p);
char** split(char* tosplit, char* spliter);

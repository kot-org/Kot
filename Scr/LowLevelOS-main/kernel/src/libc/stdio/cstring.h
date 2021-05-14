#pragma once
#include <stdint.h>
#include "../../memory/efiMemory.h"

//int
char* inttostr(uint64_t val);
char* inttostr(uint32_t val);
char* inttostr(uint16_t val);
char* inttostr(uint8_t val);
char* inttostr(int val);
char* inttostr(int64_t val);

//float
char* inttostr(double val,uint8_t decimalPlaces);
char* inttostr(double val);

//hex
char* inttohstr(uint64_t val);
char* inttohstr(uint32_t val);
char* inttohstr(uint16_t val);
char* inttohstr(uint8_t val);

//short
char* shorttostr(unsigned short* sh);

//is something
int isalpha(char _c);

//char to char*
char* chartostr(char c);

//string lenght
int strlen(char* str);
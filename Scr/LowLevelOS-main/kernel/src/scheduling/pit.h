#pragma once
#include <stdint.h>
#include <stddef.h>

extern double TimeSinceBoot;
const uint64_t PITBaseFrequency = 1193182;

void PITSleep(double seconds);
void PITSleep(uint64_t ms);

void PITSetDivisor(uint16_t divisor);
uint64_t PITGetFrequency();
void PITSetFrequency(uint64_t frequency);
void PITTick();
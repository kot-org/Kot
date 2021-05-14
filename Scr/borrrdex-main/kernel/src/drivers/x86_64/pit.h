#pragma once

#ifndef __cplusplus
#error C++ Only
#endif

#include <cstdint>

constexpr uint8_t PIT_COUNTER0_REG  = 0x40;
constexpr uint8_t PIT_COUNTER1_REG  = 0x41;
constexpr uint8_t PIT_COUNTER2_REG  = 0x42;
constexpr uint8_t PIT_COMMAND_REG   = 0x43;

// Control Word Bits
constexpr uint8_t PIT_CW_MASK_BINCOUNT  = 0x01;
constexpr uint8_t PIT_CW_MASK_MODE      = 0x0E;
constexpr uint8_t PIT_CW_MASK_RL        = 0x30;
constexpr uint8_t PIT_CW_MASK_COUNTER   = 0xC0;

// Bit 0, Binary Counter Format               XXXXXXX0
constexpr uint8_t PIT_CW_MASK_BINARY        = 0;
constexpr uint8_t PIT_CW_MASK_BCS           = 1;

// Bits 1 - 3, Operation Mode                 XXXX000X
constexpr uint8_t PIT_CW_MASK_COUNTDOWN     = 0x0;
constexpr uint8_t PIT_CW_MASK_ONESHOT       = 0x2;
constexpr uint8_t PIT_CW_MASK_RATEGEN       = 0x4;
constexpr uint8_t PIT_CW_MASK_SQRRATEGEN    = 0x6;
constexpr uint8_t PIT_CW_MASK_SWTRIGGER     = 0x8;
constexpr uint8_t PIT_CW_MASK_HWTRIGGER     = 0xA;

// Bits 4 - 5, Read / Load Mode               XX00XXXX
constexpr uint8_t PIT_CW_MASK_LATCH         = 0x00;
constexpr uint8_t PIT_CW_MASK_LSB           = 0x10;
constexpr uint8_t PIT_CW_MASK_MSB           = 0x20;
constexpr uint8_t PIT_CW_MASK_DATA          = 0x30;

// Bits 6 - 7, Counter Selection              00XXXXXX
constexpr uint8_t PIT_CW_MASK_COUNTER0      = 0x00;
constexpr uint8_t PIT_CW_MASK_COUNTER1      = 0x40;
constexpr uint8_t PIT_CW_MASK_COUNTER2      = 0x80;
constexpr uint8_t PIT_CW_MASK_COUNTERINV    = 0x90;

constexpr uint32_t PIT_BASE_FREQUENCY   = 1193181;  // True oscillation rate of the timer (Hz)
constexpr uint16_t PIT_FREQUENCY        = 1000;     // Desired interrupts per second

void pit_init();
uint32_t get_clock();
void pit_sleepms(uint64_t ms);

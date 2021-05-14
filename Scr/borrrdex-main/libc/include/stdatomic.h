#pragma once

#include "__config.h"
#include "stdint.h"

__BEGIN_DECLS

uint8_t atomic_exchange_u8(volatile uint8_t* dst, uint8_t src);
int8_t atomic_exchange_i8(volatile int8_t* dst, int8_t src);
uint16_t atomic_exchange_u16(volatile uint16_t* dst, uint16_t src);
int16_t atomic_exchange_i16(volatile int16_t* dst, int16_t src);
uint32_t atomic_exchange_u32(volatile uint32_t* dst, uint32_t src);
int32_t atomic_exchange_i32(volatile int32_t* dst, int32_t src);
uint64_t atomic_exchange_u64(volatile uint64_t* dst, uint64_t src);
int64_t atomic_exchange_i64(volatile int64_t* dst, int64_t src);

uint8_t atomic_increment_u8(volatile uint8_t* dst);
int8_t atomic_increment_i8(volatile int8_t* dst);
uint16_t atomic_increment_u16(volatile uint16_t* dst);
int16_t atomic_increment_i16(volatile int16_t* dst);
uint32_t atomic_increment_u32(volatile uint32_t* dst);
int32_t atomic_increment_i32(volatile int32_t* dst);
uint64_t atomic_increment_u64(volatile uint64_t* dst);
int64_t atomic_increment_i64(volatile int64_t* dst);

__END_DECLS
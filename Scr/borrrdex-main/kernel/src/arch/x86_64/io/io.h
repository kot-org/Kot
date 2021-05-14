#pragma once

#include <stdint.h>
#include <stdarg.h>

// Interrupt Management Configuration Register (Intel MP 1.4 p .28)
constexpr uint16_t CHIPSET_ADDRESS_REGISTER = 0x22;
constexpr uint16_t CHIPSET_DATA_REGISTER    = 0x23;
constexpr uint8_t  IMCR_REGISTER_ADDRESS    = 0x70;
constexpr uint8_t  IMCR_8259_DIRECT         = 0x00;
constexpr uint8_t  IMCR_VIA_APIC            = 0x01;

// I/O
void port_yield();

uint8_t port_read_8(uint16_t port);
uint16_t port_read_16(uint16_t port);
uint32_t port_read_32(uint16_t port);

void port_write_8(uint16_t port, uint8_t value);
void port_write_16(uint16_t port, uint16_t value);
void port_write_32(uint16_t port, uint32_t value);

void port_read(uint16_t port, uint64_t count, uint8_t* buffer);
void port_write(uint16_t port, uint64_t count, uint8_t* buffer);

// Memory mapped I/O
static inline uint8_t mmio_read_8(uint64_t addr) {
    return *((volatile uint8_t *)addr);
}

static inline uint16_t mmio_read_16(uint64_t addr) {
    return *((volatile uint16_t *)addr);
}

static inline uint32_t mmio_read_32(uint64_t addr) {
    return *((volatile uint32_t *)addr);
}

static inline uint64_t mmio_read_64(uint64_t addr) {
    return *((volatile uint64_t *)addr);
}

static inline void mmio_write_8(uint64_t addr, uint8_t value) {
    *((volatile uint8_t *)addr) = value;
}

static inline void mmio_write_16(uint64_t addr, uint16_t value) {
    *((volatile uint16_t *)addr) = value;
}

static inline void mmio_write_32(uint64_t addr, uint32_t value) {
    *((volatile uint32_t *)addr) = value;
}

static inline void mmio_write_64(uint64_t addr, uint64_t value) {
    *((volatile uint64_t *)addr) = value;
}

// Printf formatting logic for writing to endpoints
typedef void(*putc_func_t)(char, void *);
int __vprintf(putc_func_t putc, void * context, const char* fmt, va_list ap);
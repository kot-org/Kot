#pragma once

#ifndef __cplusplus
#error C++ only
#endif

#include <cstdint>

typedef struct {
    void* real_device;
    void* generic_device;

    void* descriptor;

    uint32_t io_address;

    uint32_t type;
} device_t;

constexpr uint16_t TYPECODE_TTY     = 0x201;
constexpr uint16_t TYPECODE_DISK    = 0x301;
constexpr uint16_t TYPECODE_NIC     = 0x401;

void device_init();
device_t* device_get(uint32_t typecode, uint32_t n);
int device_register(device_t* device);
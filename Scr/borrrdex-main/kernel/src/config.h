#pragma once

#ifndef __cplusplus
#error C++ Only
#endif

#include <cstdint>

constexpr uint8_t CONFIG_MAX_THREADS = 32;
constexpr uint16_t CONFIG_THREAD_STACKSIZE = 4096;
constexpr uint8_t CONFIG_MAX_CPUS = 16;
constexpr uint16_t CONFIG_MAX_SEMAPHORES = 128;
constexpr uint8_t CONFIG_MAX_FILESYSTEMS = 8;
constexpr uint16_t CONFIG_MAX_OPEN_FILES = 512;
constexpr uint8_t CONFIG_MAX_DEVICES = 128;

constexpr uint16_t CONFIG_USERLAND_STACK_SIZE = 1;

#pragma once

#ifndef __cplusplus
#error C++ Only
#endif

#include <cstdint>

struct Point {
    long x, y;
};

static constexpr uint64_t GiB(uint64_t bytes) {
    return bytes / (1024 * 1024 * 1024);
}

static constexpr uint64_t MiB(uint64_t bytes) {
    return bytes / (1024 * 1024);
}

static constexpr uint64_t KiB(uint64_t bytes) {
    return bytes / 1024;
}

static constexpr uint64_t Smallest(uint64_t bytes, const char** suffix) {
    if(bytes < 1024) {
        *suffix = " B";
        return bytes;
    }

    if(KiB(bytes) < 1024) {
        *suffix = " KiB";
        return KiB(bytes);
    }

    if(MiB(bytes) < 1024) {
        *suffix = " MiB";
        return MiB(bytes);
    }

    *suffix = " GiB";
    return GiB(bytes);
}
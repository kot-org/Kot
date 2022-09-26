#pragma once

#include <kot/sys.h>
#include <kot/types.h>
#include <kot/utils.h>
#include <kot++/printf.h>
#include <kot/utils/vector.h>

#include <srv/srv.h>

#define LBA_SIZE 0x200

static inline uint64_t ConvertBytesToLBA(uint64_t value){
    return DivideRoundUp(value, LBA_SIZE);
}

static inline uint64_t ConvertLBAToBytes(uint64_t value){
    return value * LBA_SIZE;
}
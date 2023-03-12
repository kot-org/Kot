#pragma once

#include <kot/sys.h>
#include <kot/math.h>
#include <kot/types.h>
#include <kot/utils.h>
#include <kot/utils/vector.h>

#include <kot++/json.h>
#include <kot++/printf.h>

#include <srv/srv.h>
#include <vfs/vfs.h>
#include <system/system.h>

using namespace std;

#define LBA_SIZE 0x200

static inline uint64_t ConvertBytesToLBA(uint64_t value){
    return value >> 9;
}

static inline uint64_t ConvertLBAToBytes(uint64_t value){
    return value << 9;
}
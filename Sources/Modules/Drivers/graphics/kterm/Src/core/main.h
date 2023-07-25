#pragma once

#include <errno.h>
#include <sys/ioctl.h>

#include <kot/sys.h>
#include <kot/utils.h>
#include <kot/utils/vector.h>
#include <kot/uisd/srvs/system.h>
#include <kot/uisd/srvs/storage.h>
#include <kot/uisd/srvs/graphics.h>

#define TERMINAL_MAX    1


extern void* TableConverter;
extern size64_t TableConverterCharCount;

void KeyboardInterruptEntry(uint64_t Key);
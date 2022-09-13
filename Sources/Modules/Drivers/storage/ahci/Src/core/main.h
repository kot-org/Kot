#pragma once

#include <kot/sys.h>
#include <kot/math.h>
#include <kot/memory.h>
#include <kot/atomic.h>
#include <kot++/printf.h>
#include <kot/uisd/srvs/pci.h>
#include <controller/controller.h>

extern process_t Proc;

void AddDevice(class Device* Device);
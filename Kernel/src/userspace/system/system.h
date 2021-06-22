#pragma once

#include <stdint.h>
#include "../../graphics.h"
#include "../../lib/stdio.h"
#include "../../hardware/cpu/cpu.h"
#include "../../interrupts/interrupts.h"
extern bool wait;
extern "C" void EnableSystemCall();
extern "C" void syscall_entry();

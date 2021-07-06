#pragma once
#include <stdint.h>
#include "../system/system.h"
#include "../../paging/pageTableManager.h"
#include "../../paging/PageFrameAllocator.h"

extern "C" void IdleTask();
extern "C" void task1();
extern "C" void task2();
extern "C" void task3();
extern "C" void task4();

extern "C" void SaveTSS(uint64_t CoreID);
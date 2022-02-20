#pragma once
#include <lib/types.h>
#include <memory/paging/pageTableManager.h>
#include <memory/paging/PageFrameAllocator.h>

extern "C" void IdleTask();

extern "C" void LaunchUserSpace();
#pragma once

#include <core/main.h>

extern bool* IsIRQEventsFree;

void InitializeInterrupts(struct KernelInfo* kernelInfo);
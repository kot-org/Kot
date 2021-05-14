#pragma once

#ifndef __cplusplus
#error C++ Only
#endif

#include "thread.h"

void process_start(const char* executable, const char** argv);
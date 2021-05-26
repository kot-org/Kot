#pragma once

#include "../../graphics.h"
#include "../va_args.h"
#include "../limits.h"
#include "cstr.h"

void printf(const char* str, ...);
void sprintf(char* buffer,const char* str, ...);
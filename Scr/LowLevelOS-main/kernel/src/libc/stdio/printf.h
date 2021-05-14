#pragma once
#include "../va_args.h"
#include "../../drivers/display/displaydriver.h"
#include "cstring.h"
#include "../limits.h"

void printf(const char* str, ...);
void sprintf(char* buffer,const char* str, ...);
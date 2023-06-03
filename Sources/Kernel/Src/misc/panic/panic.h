#pragma once
#include <logs/logs.h>
#include <arch/x86-64.h>

#ifndef NDEBUG
#define assert(expression) if(!(expression)) KernelPanic("assert failed : "#expression);
#else
#define assert(expression) ((void)0)
#endif

void KernelPanic(char* message);
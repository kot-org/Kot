#pragma once


void Panic(const char* panicMessage);
__attribute__((noreturn)) void AssertionPanic(const char* file, int line, const char* message);

#define KERNEL_ASSERT(condition) \
if(!(condition)) \
AssertionPanic(__FILE__, __LINE__, #condition)

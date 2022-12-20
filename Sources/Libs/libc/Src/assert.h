
#ifndef _ASSERT_H
#define _ASSERT_H 1

#include <kot/types.h>
#include <kot/cstring.h>

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef NDEBUG
#define assert(expression) _assert(expression, #expression, __FILE__, __LINE__, __FUNCTION__)
#else
#define assert(expression) ((void)0)
#endif

void _assert(int expression, const char* expression_name, const char* file, int line, const char* function);

#if defined(__cplusplus)
}
#endif

#endif
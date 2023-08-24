#ifndef _ASSERT_H
#define _ASSERT_H 1

void __assert_fail(const char *__assertion, const char *__file, unsigned int __line, const char *__function);

#define assert(expr) ((expr) ? (void)0 : __assert_fail(#expr, __FILE__, __LINE__, __func__))

#endif // _ASSERT_H
#include <lib/log.h>
#include <impl/panic.h>

void __assert_fail(const char *__assertion, const char *__file, unsigned int __line, const char *__function) {
    panic("Assertion failed: %s, file %s, function %s, line %d", __assertion, __file, __function, __line);
}
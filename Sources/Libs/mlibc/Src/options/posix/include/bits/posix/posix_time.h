#ifndef MLIBC_POSIX_TIME_H
#define MLIBC_POSIX_TIME_H

#include <stddef.h>
#include <bits/posix/timeval.h>
#include <bits/posix/locale_t.h>

#define TIMER_ABSTIME 1

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int utimes(const char *, const struct timeval[2]);

// Not standardized, Linux and BSDs have it
int futimes(int, const struct timeval[2]);

size_t strftime_l(char *__restrict dest, size_t max_size, const char *__restrict format, const struct tm *__restrict tm, locale_t);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // MLIBC_POSIX_TIME_H

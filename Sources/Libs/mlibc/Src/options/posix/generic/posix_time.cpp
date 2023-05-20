#include <bits/posix/posix_time.h>
#include <bits/ensure.h>
#include <mlibc/posix-sysdeps.hpp>
#include <errno.h>
#include <stdlib.h>

#include <frg/small_vector.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>
#include <mlibc/rtdl-config.hpp>

int utimes(const char *filename, const struct timeval times[2]) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_utimensat, -1);
	struct timespec time[2];
	if(times == nullptr) {
		time[0].tv_sec = UTIME_NOW;
		time[0].tv_nsec = UTIME_NOW;
		time[1].tv_sec = UTIME_NOW;
		time[1].tv_nsec = UTIME_NOW;
	} else {
		time[0].tv_sec = times[0].tv_sec;
		time[0].tv_nsec = times[0].tv_usec * 1000;
		time[1].tv_sec = times[1].tv_sec;
		time[1].tv_nsec = times[1].tv_usec * 1000;
	}

	if (int e = mlibc::sys_utimensat(AT_FDCWD, filename, time, 0); e) {
		errno = e;
		return -1;
	}

	return 0;
}

int futimes(int, const struct timeval[2]) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

size_t strftime_l(char *__restrict dest, size_t max_size, const char *__restrict format, const struct tm *__restrict tm, locale_t){
	mlibc::infoLogger() << "mlibc: strftime_l ignores locales" << frg::endlog;
	return strftime(dest, max_size, format, tm);
}
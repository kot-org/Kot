#ifndef _KERNEL_H
#define _KERNEL_H 1

#define KERNEL_MAJOR 0
#define KERNEL_MINOR 1
#define KERNEL_VERSION "waterfall"
#define KERNEL_BRANCH "dev"

#if defined (__x86_64__)
# define KERNEL_ARCH "amd64"
#elif defined (__aarch64__)
# define KERNEL_ARCH "aarch64"
#elif defined (__riscv) && (__riscv_xlen == 64)
# define KERNEL_ARCH "riscv64"
#else
# define KERNEL_ARCH "unknown"
#endif

#endif // _KERNEL_H

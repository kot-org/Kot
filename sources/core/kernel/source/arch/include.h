#ifndef _ARCH_INCLUDE_H
#define _ARCH_INCLUDE_H 1

#if defined (__x86_64__)
# define ARCH_INCLUDE(header_relative_path) <arch/amd64/header_relative_path>
#elif defined (__aarch64__)
# define ARCH_INCLUDE(header_relative_path) <arch/aarch64/header_relative_path>
#elif defined (__riscv) && (__riscv_xlen == 64)
# define ARCH_INCLUDE(header_relative_path) <arch/riscv64/header_relative_path>
#else
# error "unknown arch"
#endif

#endif // _ARCH_INCLUDE_H

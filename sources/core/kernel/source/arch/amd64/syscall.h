#ifndef _AMD64_SYSCALL_H
#define _AMD64_SYSCALL_H

#include <stdint.h>

void syscall_enable(uint16_t kernel_segment, uint16_t user_segment);

#endif // _AMD64_SYSCALL_H
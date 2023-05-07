//===-------------------------- abort_message.h-----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef __ABORT_MESSAGE_H_
#define __ABORT_MESSAGE_H_

#include "cxxabi.h"

#ifdef __cplusplus
extern "C" {
#endif

// Our printf is a definition, which is messing up this abort message format string.
#ifdef printf
#undef printf
#endif

_LIBCXXABI_HIDDEN _LIBCXXABI_NORETURN void
abort_message(const char *format, ...) __attribute__((format(printf, 1, 2)));

#ifdef __cplusplus
}
#endif

#endif


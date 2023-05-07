//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP_SUPPORT_BAREMETAL_XLOCALE_H
#define _LIBCPP_SUPPORT_BAREMETAL_XLOCALE_H

#include <cstdlib>
#include <clocale>
#include <cwctype>
#include <ctype.h>
#if !defined(__NEWLIB__) || __NEWLIB__ < 2 || \
    __NEWLIB__ == 2 && __NEWLIB_MINOR__ < 5
#include "__nop_locale_mgmt.h"
#endif

#include "__posix_l_fallback.h"
#include "__strtonum_fallback.h"

#endif

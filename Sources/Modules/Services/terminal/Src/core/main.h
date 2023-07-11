#pragma once

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/ioctl.h>

#include <kot/sys.h>
#include <kot/uisd.h>
#include <kot/utils.h>
#include <kot/utils/vector.h>
#include <kot-graphics/font.h>

#include <kot++/string.h>

#include <kot/uisd/srvs/hid.h>
#include <kot/uisd/srvs/time.h>
#include <kot/uisd/srvs/system.h>

#include <srv/srv.h>
#include <core/core.h>

extern void* TableConverter;
extern size64_t TableConverterCharCount;
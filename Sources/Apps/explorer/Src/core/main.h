#pragma once

#include <kot/sys.h>
#include <assert.h>
#include <kot/uisd/srvs/system.h>
#include <kot/uisd/srvs/storage.h>

#include <kot++/string.h>

#include <kot-ui++/window.h>

#include <kot-audio++/stream.h>

struct Explorer_File_Data{
    bool IsFile;
    char* Name;
};
#pragma once

#include <kot/sys.h>
#include <kot/assert.h>
#include <kot/uisd/srvs/storage.h>

#include <kot++/string.h>

#include <kot-ui++/window.h>

struct Explorer_File_Data{
    bool IsFile;
    char* Name;
};
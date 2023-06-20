#pragma once

#include <stdio.h>
#include <limits.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#include <kot/sys.h>

#include <kot-ui/core.h>
#include <kot-ui/renderer.h>

struct Explorer_File_Data{
    bool IsFile;
    char* Name;
};
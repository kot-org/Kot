#pragma once

#include <stdio.h>
#include <limits.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <kot/sys.h>
#include <kot/launch.h>

#include <kot++/new.h>

#include <kot-audio++/stream.h>

#include <kot-ui/core.h>
#include <kot-ui/renderer.h>

#include <kot/uisd/srvs/audio.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}
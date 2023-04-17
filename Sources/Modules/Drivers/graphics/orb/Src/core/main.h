#pragma once

#include <kot/sys.h>
#include <kot/bits.h>
#include <kot/math.h>
#include <kot/heap.h>
#include <kot/uisd.h>
#include <kot/utils.h>
#include <kot/atomic.h>
#include <kot/assert.h>
#include <kot/utils/vector.h>
#include <kot/uisd/srvs/hid.h>
#include <kot/uisd/srvs/time.h>
#include <kot/uisd/srvs/system.h>
#include <kot/uisd/srvs/storage.h>
#include <kot/uisd/srvs/graphics.h>

#include <kot++/printf.h>

#include <kot-graphics/utils.h>
#include <kot-graphics/event.h>

class Context;

#include <srv/srv.h>
#include <boot/boot.h>
#include <mouse/mouse.h>
#include <render/render.h>
#include <window/window.h>
#include <monitor/monitor.h>
#include <desktop/desktop.h>

class orbc;
class windowc;
class monitorc;
class desktopc;
class renderc;
class mousec;


extern process_t ShareableProcess;

class orbc{
    public:
        orbc();

        renderc* Render; 
        desktopc* Desktop;    
        mousec* Mouse;
        monitorc* Monitor;
};
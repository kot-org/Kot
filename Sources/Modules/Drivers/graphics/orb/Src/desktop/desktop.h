#pragma once

#include <core/main.h>

class orbc;
class windowc;
class monitorc;
class desktopc;
class renderc;
class mousec;

class desktopc{
    public:
        desktopc(orbc* Parent);
        KResult AddMonitor(monitorc* Monitor);
        KResult RemoveMonitor(monitorc* Monitor);
        KResult Update(monitorc* Monitor);
};
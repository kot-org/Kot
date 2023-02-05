#pragma once

#include <core/main.h>

extern vector_t* Monitors;
extern windowc* FirstWindowNode;
extern uint64_t RenderMutex;

void RenderWindows();
void UpdateAllEvents();
void ThreadRender();
KResult StartRender();
KResult StopRender();
KResult InitializeRender();
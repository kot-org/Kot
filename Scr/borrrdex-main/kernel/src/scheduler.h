#pragma once

#ifndef __cplusplus
#error C++ only
#endif

#include "thread.h"

void scheduler_init();
void scheduler_add_ready(tid_t t);
void scheduler_mark_finished();
void scheduler_schedule();
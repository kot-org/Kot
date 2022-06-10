#pragma once

#include <main.h>

KResult MouseInitalize();

void MouseWait();
void MouseWrite(uint8_t value);

void MouseHandler(enum EventType type, void* data);
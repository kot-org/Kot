#pragma once

#include <stddef.h>

struct Framebuffer {
	void* baseAddress;
	size_t bufferSize;
	unsigned width;
	unsigned height;
	unsigned pixelsPerScanline;
};
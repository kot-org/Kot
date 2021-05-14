#pragma once
#include "stdint.h"
#include "stddef.h"
#include "../../libc/stdio.h"
#include "../../libc/math.h"
#include "../../libc/time.h"
#include "../../memory/efiMemory.h"
#include "../../memory/PageFrameAllocator.h"
#include "../../misc/psf1.h"

typedef struct {
	void* BaseAddr;
	long long BufferSize;
	unsigned int Width;
	unsigned int Height;
	unsigned int PixelPerScanLine;
} DisplayBuffer;

class DisplayDriver {
public:

//drawing pixels
void putpix(int x,int y);
void putpix(int x,int y,unsigned int clr);
void putrect(int x,int y,int h,int w);
void putrect(int xx,int yy,int h,int w,unsigned int clr);
void putbox(int xx,int yy,int h,int w);
void putbox(int xx,int yy,int h,int w,unsigned int clr);
void scroll();

//text rendering
void puts(const char* ch);
void putc(char ch,unsigned int xx,unsigned int yy);
void putc(char ch);
void boxedputs(unsigned int highcolour,const char* ch);
void highlightputs(unsigned int highcolour,const char* ch);

//cursor
void setCursorPos(int x,int y);
void cursorNewLine();
void advanceCursor();

//misc
void clearScreen(unsigned int colour);
void setColour(unsigned int colo);
void checkScroll();
void fillGarbage();
void fillRainbow();

//initializare
void InitDisplayDriver(DisplayBuffer* framebuf, PSF1_FONT* font);
void InitDoubleBuffer(DisplayBuffer* f);

//ceva geturi
uint64_t getWidth();
uint64_t getHeight();

//chestii de double buffer
void update();

//variabile
DisplayBuffer* secondFrameBuffer;
DisplayBuffer* globalFrameBuffer;
PSF1_FONT* globalFont;
Point CursorPos;
uint64_t colour;
};

extern DisplayDriver* GlobalDisplay;
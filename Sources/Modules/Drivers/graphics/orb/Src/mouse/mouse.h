#include <kot/sys.h>
#include <kot/math.h>
#include <kot/types.h>

#include <kot/uisd/srvs/hid.h>

#include <kot-graphics++/utils.h>

#define CursorWidth     16
#define CursorHeight    19

extern Point_t CursorPosition;
extern Point_t CursorMaxPosition;

void InitializeCursor();
void CursorInterrupt(int64_t x, int64_t y, int64_t z, uint64_t status);

void DrawCursor(Graphic::framebuffer_t* fb, uint8_t* Mask, uint32_t Color[CursorHeight][CursorWidth]);
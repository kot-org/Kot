#include <kot/sys.h>
#include <kot/math.h>
#include <kot/types.h>

#include <kot/uisd/srvs/hid.h>

#define CursorWidth     10
#define CursorHeight    10

extern Point_t CursorPosition;
extern Point_t CursorMaxPosition;

void InitializeCursor();
void CursorInterrupt(int64_t x, int64_t y, int64_t z, uint64_t status);
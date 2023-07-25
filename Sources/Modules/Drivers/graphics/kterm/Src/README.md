# Port of the Limine bootloader terminal

Normally this terminal is provided by the Limine bootloader and can be used by the kernel during early boot.

It's an extremely fast with a complete "terminfo"/vt100 implementation. There really isn't a reason not to use this terminal.

The only issue here is that it's merely an *early boot console*, and it's located in conventional, lower-half memory.

Once you get to userspace you'll find it very annoying to try and map memory *around* it considering that terminal shouldn't be in lower-half memory in the first place!

That's what this port is for.
You should be able to include it into your kernel and use it just fine.

Please let us know if any issues arise, thank you!

## Features
* Everything that Limine terminal supports
* Multiple terminals

## Usage

1. First off, choose a font from fonts/ folder (binary or c array) or create your own and load it in your os (link it directly to the kernel, convert it to an array, load it from filesystem, as a module, etc)

2. To initialize the terminal, include `term.h` and provide some basic functions declared in the header file (Example shown below)

3. Create new term_t and run `term_init(term, arguments);` (If you set bios to false, you will not be able to use text mode)

4. To use vbe mode with framebuffer, run `term_vbe(term, arguments);` (Example shown below)

5. To use text mode, run `term_textmode(term);`

Note: There also are C++ wrappers for term_t and image_t structures (cppterm_t and cppimage_t) in `source/cpp/` directory

## Example
```c
#include "term.h"

void *alloc_mem(size_t size)
{
   // Allocate memory
   // memset() memory to zero
}
void free_mem(void *ptr, size_t size)
{
   // Free memory
}
void *memcpy(void *dest, const void *src, size_t len)
{
   // Memcpy
}
void *memset(void *dest, int ch, size_t n)
{
   // Memset
}

void callback(term_t*, uint64_t, uint64_t, uint64_t, uint64_t)
{
    // Handle callback
}

struct framebuffer_t frm = {
   address, // Framebuffer address
   width, // Framebuffer width
   height, // Framebuffer height
   pitch // Framebuffer pitch
};

struct font_t font = {
   font_address, // Address of the font
   8, // Font width
   16, // Font height
   1, // Character spacing
   1, // Font scaling x
   1 // Font scaling y
};

struct style_t style = {
   DEFAULT_ANSI_COLOURS, // Default terminal palette
   DEFAULT_ANSI_BRIGHT_COLOURS, // Default terminal bright palette
   DEFAULT_BACKGROUND, // Background colour
   DEFAULT_FOREGROUND, // Foreground colour
   DEFAULT_MARGIN, // Terminal margin
   DEFAULT_MARGIN_GRADIENT // Terminal margin gradient
};

struct image_t image;
image_open(&image, bmpBackgroundAddress, size);
struct background_t back = {
   &image, // Background. Set to NULL to disable background
   STRETCHED, // STRETCHED, CENTERED or TILED
   DEFAULT_BACKGROUND // Terminal backdrop colour
};

struct term_t term;
term_init(&term, callback, bootedInBiosMode, TERM_TABSIZE);

// VBE mode
// In VBE mode you can create more terminals for different framebuffers
term_vbe(&term, frm, font, style, back);
term_write(&term, "Hello, World!", 13);

// Text mode
term_textmode(&term);
term_write(&term, "Hello, World!", 13);
```

Based on: https://github.com/limine-bootloader/limine

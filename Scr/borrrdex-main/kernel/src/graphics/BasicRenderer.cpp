#include "BasicRenderer.h"
#include "SimpleFont.h"
#include "../uefi/FrameBuffer.h"
#include "arch/x86_64/io/io.h"
#include <cstdarg>

constexpr uint32_t CURSOR_COLOR = 0xff00dd00;
constexpr uint8_t TABSTOP = 4;

BasicRenderer* GlobalRenderer;

void printf_putc_callback(char ch, void* ctx) {
    ((BasicRenderer *)ctx)->PutChar(ch);
}

void BasicRenderer::tick(datetime_t* tm) {
    _cursorVisible = !_cursorVisible;
    if(_cursorVisible) {
        ShowCursor();
    } else  {
        ClearCursor();
    }
}

bool BasicRenderer::HasChar() {
    unsigned* pixPtr = (unsigned *)_targetFrameBuffer->baseAddress;
    for(unsigned i = 0; i < 8; i++) {
        for(unsigned j = 0; j < 16; j++) {
            unsigned col = *(unsigned*)(pixPtr + CursorPosition.x + (CursorPosition.y * _targetFrameBuffer->pixelsPerScanline));
            if(col != _background) {
                return true;
            }
        }
    }

    return false;
}

BasicRenderer::BasicRenderer(Framebuffer* targetFrameBuffer, PSF1_FONT* font) 
    :_targetFrameBuffer(targetFrameBuffer)
    ,_psf1Font(font) {
        
}

void BasicRenderer::Printf(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    __vprintf(printf_putc_callback, this, fmt, ap);

    va_end(ap);
}

void BasicRenderer::PrintN(const char* str, uint64_t len) {
    const char* chr = str;
    while(*chr && len-- > 0) {
        PutChar(*chr++);
    }
}

void BasicRenderer::ClearCurrent() {
    unsigned xOff = CursorPosition.x;
    unsigned yOff = CursorPosition.y;
    unsigned* pixPtr = (unsigned *)_targetFrameBuffer->baseAddress;
    for(unsigned long y = yOff; y < yOff + 16; y++) {
        for(unsigned long x = xOff; x < xOff + 8; x++) {
            *(unsigned*)(pixPtr + x + (y * _targetFrameBuffer->pixelsPerScanline)) = _background;
        }
    }
}

void BasicRenderer::ShowCursor() {
    if(!_cursorVisible) {
        return;
    }
    
    unsigned xOff = CursorPosition.x;
    unsigned yOff = CursorPosition.y;
    unsigned* pixPtr = (unsigned *)_targetFrameBuffer->baseAddress;
    for(unsigned long y = yOff; y < yOff + 16; y++) {
        for(unsigned long x = xOff; x < xOff + 8; x++) {
            unsigned* curColor = (unsigned*)(pixPtr + x + (y * _targetFrameBuffer->pixelsPerScanline));
            if(*curColor == _background) {
                *curColor = CURSOR_COLOR;
            }
        }
    }
}

void BasicRenderer::ClearCursor() {
    unsigned xOff = CursorPosition.x;
    unsigned yOff = CursorPosition.y;
    unsigned* pixPtr = (unsigned *)_targetFrameBuffer->baseAddress;
    for(unsigned long y = yOff; y < yOff + 16; y++) {
        for(unsigned long x = xOff; x < xOff + 8; x++) {
            unsigned* curColor = (unsigned*)(pixPtr + x + (y * _targetFrameBuffer->pixelsPerScanline));
            if(*curColor == CURSOR_COLOR) {
                *curColor = _background;
            }
        }
    }
}

void BasicRenderer::PutChar(char chr) {
    ClearCurrent();

    PutCharAt(chr, CursorPosition.x, CursorPosition.y);
    if(chr == '\r' || chr == '\n') {
        return;
    }

    if(CursorPosition.x + 8 == _targetFrameBuffer->width) {
        Next();
    } else if(chr != '\t') {
        CursorPosition.x += 8;
    }
}

void BasicRenderer::PutCharAt(char chr, unsigned xOff, unsigned yOff) {
    if(chr == '\r') {
        CursorPosition.x = 0;
        return;
    }

    if(chr == '\n') {
        // Unix style behavior
        Next();
        return;
    }

    if(chr == '\t') {
        CursorPosition.x += 8;
        while(CursorPosition.x % (TABSTOP * 8)) {
            CursorPosition.x += 8;
        }

        return;
    }

    unsigned* pixPtr = (unsigned *)_targetFrameBuffer->baseAddress;
    char* fontPtr = (char *)_psf1Font->glyphBuffer + (chr * _psf1Font->psf1_header->charsize);
    for(unsigned long y = yOff; y < yOff + 16; y++) {
        for(unsigned long x = xOff; x < xOff + 8; x++) {
            if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0) {
                *(unsigned*)(pixPtr + x + (y * _targetFrameBuffer->pixelsPerScanline)) = _color;
            }
        }

        fontPtr++;
    }
}

void BasicRenderer::Next() {
    ClearCursor();

    CursorPosition.x = 0;
    CursorPosition.y += 16;
}

void BasicRenderer::Clear() {
    uint64_t fbBase = (uint64_t)_targetFrameBuffer->baseAddress;
    uint64_t bytesPerScanline = _targetFrameBuffer->pixelsPerScanline * 4;
    uint64_t fbHeight = _targetFrameBuffer->height;
    uint64_t fbSize = _targetFrameBuffer->bufferSize;

    for(int verticalScanLine = 0; verticalScanLine < fbHeight; verticalScanLine++) {
        uint64_t pixPtrBase = fbBase + (bytesPerScanline * verticalScanLine);
        for(uint32_t* pixPtr = (uint32_t *)pixPtrBase; pixPtr < (uint32_t *)(pixPtrBase + bytesPerScanline); pixPtr++) {
            *pixPtr = _background;
        }
    }
}

void BasicRenderer::ClearChar() {
    ClearCurrent();

    CursorPosition.x -= 8;
    if(CursorPosition.x < 0) {
        CursorPosition.x = _targetFrameBuffer->width - 8;
        CursorPosition.y -= 16;
        if(CursorPosition.y < 0) {
            CursorPosition.y = 0;
            CursorPosition.x = 0;
        }
    }

    ClearCurrent();
    ShowCursor();
}

void BasicRenderer::Up() {
    if(CursorPosition.y == 0) {
        return;
    }

    ClearCursor();
    CursorPosition.y -= 16;
    ShowCursor();
}

void BasicRenderer::Down() {
    if(CursorPosition.y == _targetFrameBuffer->height - 16) {
        return;
    }

    ClearCursor();
    CursorPosition.y += 16;
    ShowCursor();
}

void BasicRenderer::Left() {
    if(CursorPosition.x == 0) {
        return;
    }

    ClearCursor();
    CursorPosition.x -= 8;
    ShowCursor();
}

void BasicRenderer::Right() {
    if(CursorPosition.x == _targetFrameBuffer->width - 8) {
        return;
    }

    ClearCursor();
    CursorPosition.x += 8;
    ShowCursor();
}

unsigned BasicRenderer::Width() const {
    return _targetFrameBuffer->width;
}

unsigned BasicRenderer::Height() const {
    return _targetFrameBuffer->height;
}
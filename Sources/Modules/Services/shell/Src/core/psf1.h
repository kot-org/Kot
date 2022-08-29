#pragma once

#include <kot/heap.h>

#define PSF1_MAGIC     0x3604

#define PSF1_MODE512    0x01
#define PSF1_MODEHASTAB 0x02
#define PSF1_MODEHASSEQ 0x04
#define PSF1_MAXMODE    0x05

#define PSF1_SEPARATOR  0xFFFF
#define PSF1_STARTSEQ   0xFFFE

typedef struct {
    uint16_t magic;     /* Magic number */
    uint8_t mode;       /* PSF font mode */
    uint8_t charsize;   /* Character size */
} psf1_header;

typedef struct {
    psf1_header* header;
    uintptr_t glyphBuffer;
} psf1_font;

psf1_font* psf1_parse(uintptr_t bytes) {
    psf1_font* font = (psf1_font*) calloc(sizeof(psf1_font));
    font->header = (psf1_header*) bytes;
    font->glyphBuffer = (uintptr_t)((uint64_t) bytes + sizeof(psf1_header));
    return font;
}

void psf1_putchar(psf1_font* font, uintptr_t fb_addr, uint32_t fb_height, char chr, uint32_t* xOff, uint32_t* yOff, uint32_t colour) {
    uint32_t* pix_ptr = (uint32_t*) fb_addr;
    uint8_t* font_ptr = (uint8_t*) font->glyphBuffer + (chr * font->header->charsize);
    for (uint64_t y = *yOff; y < *yOff + 16; y++) {
        for (uint64_t x = *xOff; x < *xOff + 8; x++) {
            if ((*font_ptr & (0b10000000 >> (x - *xOff))) > 0) {
                *(uint32_t*)(pix_ptr + (x + (y * fb_height))) = colour;
            }
        }
        font_ptr++;
    }
    *(xOff) = *(xOff)+8;
}

void psf1_print(psf1_font* font, uintptr_t fb_addr, uint32_t fb_height, char* str, uint32_t* xOff, uint32_t* yOff, uint32_t colour) {

    for (char* c = str; *c != '\0'; c++) {
        if (*c == '\n') {
            *(yOff) = *(yOff) + 16;
            *(xOff) = 0;
            continue;
        } else if (*c == '\r') {
            *(xOff) = 0;
            continue;
        }
        psf1_putchar(font, fb_addr, fb_height, *c, xOff, yOff, colour);
    }
    
}


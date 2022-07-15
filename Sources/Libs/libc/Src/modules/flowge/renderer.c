#include <kot/modules/flowge/renderer.h>
#include <kot/math.h>

void clear(screen_t* screen) {
    memset(screen->fb_addr, 0x00, screen->fb_size);
}

void putPixel(screen_t* screen, uint32_t x, uint32_t y, uint32_t colour) {
    uint8_t *fb = screen->fb_addr;
    uint64_t index = (x * screen->btpp) + (y * screen->bps);
    fb[index + 2] = (colour >> 16) & 255;
    fb[index + 1] = (colour >> 8) & 255;
    fb[index] = colour & 255; 
}

bool pixelExist(screen_t* screen, uint32_t x, uint32_t y) {
    // todo !!
    return true;
}

uint32_t getPixel(screen_t* screen, uint32_t x, uint32_t y) {
    uint8_t *fb = screen->fb_addr;
    uint64_t index = (x * screen->btpp) + (y * screen->bps);
    return fb[index];
}

void drawLine(screen_t* screen, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t colour) {

    uint8_t dX = abs(x2-x1);
    uint8_t dY = abs(y2-y1);

    int64_t s1 = sgn(x2-x1);
    int64_t s2 = sgn(y2-y1);

    uint8_t interchange = 0;

    if (dY > dX) {
        uint8_t temp = dX;
        dX = dY;
        dY = temp;
        interchange = 1;
    }

    int64_t E = 2 * (dY-dX);
    int64_t A = 2 * dY;
    int64_t B = 2 * dY - 2 * dX;

    uint32_t x = x1;
    uint32_t y = y1;

    putPixel(screen, x, y, colour);

    for (size_t i = 1; i < dX; i++) {

        if (E < 0) {
            if (interchange == 1) {
                y += s2;
            } else {
                x += s1;
                E += A;
            }
        } else {
            y += s2;
            x += s1;
            E += B;
        }

        putPixel(screen, x, y, colour);

    }

}

void fillRect(screen_t* screen, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t colour) {
    // !!! todo optimize fillRect !!!
    // uint8_t *fb = screen->fb_addr;
    // uint64_t index = (x * screen->btpp) + (y * screen->bps);
    // fb[index + 2] = (colour >> 16) & 255;
    // fb[index + 1] = (colour >> 8) & 255;
    // fb[index] = colour & 255; 
    // for (uint32_t i = 1; i < h; i++) {
    //     for (uint32_t j = 1; j < w; j++) {
    //         index += j * screen->btpp;
    //         fb[index + 2] = (colour >> 16) & 255;
    //         fb[index + 1] = (colour >> 8) & 255;
    //         fb[index] = colour & 255; 
    //     }
    //     index += i * screen->bps;
    // }
    for (uint32_t i = 0; i < h; i++) {
        for (uint32_t j = 0; j < w; j++) {
            putPixel(screen, x + j, y + i, colour);
        }
    }
}

void drawRect(screen_t* screen, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t colour) {
    drawLine(screen, x, y, x+w, y, colour);
    drawLine(screen, x, y, x, y+h, colour);
    drawLine(screen, x+w, y, x, y+h, colour);
    drawLine(screen, x, y+h, x+w, y+h, colour);
}
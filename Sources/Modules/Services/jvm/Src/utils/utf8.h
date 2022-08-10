#include <kot/types.h>

namespace utf8 {

    char32_t nextChar(const uint8_t* str, uint32_t& idx) {

        uint8_t c1 = str[idx];
        ++idx;

        char32_t utf8c;

        if (((c1 >> 6) & 0b11) == 0b11) {

            uint8_t c2 = str[idx];
            ++idx;

            if ((c1 >> 5) & 1) {

                uint8_t c3 = str[idx];
                ++idx;

                if ((c1 >> 4) & 1) {
                    uint8_t c4 = str[idx];
                    ++idx;
                    utf8c = ((c4 & 0b00000111) << 18) | ((c3 & 0b00111111) << 12) |
                            ((c2 & 0b00111111) << 6) | (c1 & 0b00111111);
                } else {
                    utf8c = ((c3 & 0b00001111) << 12) | ((c2 & 0b00111111) << 6) |
                            (c1 & 0b00111111);
                }

            } else {
                utf8c = ((c1 & 0b00011111) << 6) | (c2 & 0b00111111);
            }

        } else {
            utf8c = c1 & 0b01111111;
        }

        return utf8c;

    }

    char* decode(const uint8_t* bytes) {

        uint32_t idx = 0;
        uint32_t c;
        while ((c = utf8::nextChar(bytes, idx)) != 0) {

            

        }

    }

}
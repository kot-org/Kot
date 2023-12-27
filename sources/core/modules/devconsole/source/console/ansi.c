#include "ansi.h"

#include "console.h"

uint16_t ansi_read(const char* code) {
    uint16_t i = 0;

    // ignore the start and the style (regular, bold,...)
    if(code[i] == ANSI_START || code[i+1] == ANSI_SEPARATOR) i++;

    while(code[i++] != ANSI_END) {

        switch(code[i])
        {
            case ANSI_RESET:
                devconsole_set_bg_color(DEFAULT_BG_COLOR);
                devconsole_set_fg_color(DEFAULT_FG_COLOR);
                break;

            case ANSI_FG:
            {
                switch(code[i+1])
                {
                    case COLOR_BLACK:
                        devconsole_set_fg_color(0x000000);
                        break;
                    case COLOR_RED:
                        devconsole_set_fg_color(0xCD3131);
                        break;
                    case COLOR_GREEN:
                        devconsole_set_fg_color(0x0DBC79);
                        break;
                    case COLOR_YELLOW:
                        devconsole_set_fg_color(0xE5E510);
                        break;
                    case COLOR_BLUE:
                        devconsole_set_fg_color(0x2472C8);
                        break;
                    case COLOR_PURPLE:
                        devconsole_set_fg_color(0xBC3FBC);
                        break;
                    case COLOR_CYAN:
                        devconsole_set_fg_color(0x11A8CD);
                        break;
                    case COLOR_WHITE:
                        devconsole_set_fg_color(0xCFCFCF);
                        break;

                    default:
                        break;
                }
                break;
            }

            case ANSI_BG:
            {
                switch(code[i+1])
                {
                    case COLOR_BLACK:
                        devconsole_set_bg_color(0x000000);
                        break;
                    case COLOR_RED:
                        devconsole_set_bg_color(0xCD3131);
                        break;
                    case COLOR_GREEN:
                        devconsole_set_bg_color(0x0DBC79);
                        break;
                    case COLOR_YELLOW:
                        devconsole_set_bg_color(0xE5E510);
                        break;
                    case COLOR_BLUE:
                        devconsole_set_bg_color(0x2472C8);
                        break;
                    case COLOR_PURPLE:
                        devconsole_set_bg_color(0xBC3FBC);
                        break;
                    case COLOR_CYAN:
                        devconsole_set_bg_color(0x11A8CD);
                        break;
                    case COLOR_WHITE:
                        devconsole_set_bg_color(0xCFCFCF);
                        break;

                    default:
                        break;
                }
                break;
            }

            case ANSI_BRIGHT_FG:
            {
                switch(code[i+1])
                {
                    case COLOR_BLACK:
                        devconsole_set_fg_color(0x666666);
                        break;
                    case COLOR_RED:
                        devconsole_set_fg_color(0xF14C4C);
                        break;
                    case COLOR_GREEN:
                        devconsole_set_fg_color(0x23D18B);
                        break;
                    case COLOR_YELLOW:
                        devconsole_set_fg_color(0xF5F543);
                        break;
                    case COLOR_BLUE:
                        devconsole_set_fg_color(0x3B8EEA);
                        break;
                    case COLOR_PURPLE:
                        devconsole_set_fg_color(0xD670D6);
                        break;
                    case COLOR_CYAN:
                        devconsole_set_fg_color(0x29B8DB);
                        break;
                    case COLOR_WHITE:
                        devconsole_set_fg_color(0xFFFFFF);
                        break;

                    default:
                        break;
                }
                break;
            }

            case ANSI_BRIGHT_BG:
            {
                i++; // we increment by 1 to take the last digit (color) because bright_bg is > 100

                switch(code[i+1])
                {
                    case COLOR_BLACK:
                        devconsole_set_bg_color(0x666666);
                        break;
                    case COLOR_RED:
                        devconsole_set_bg_color(0xF14C4C);
                        break;
                    case COLOR_GREEN:
                        devconsole_set_bg_color(0x23D18B);
                        break;
                    case COLOR_YELLOW:
                        devconsole_set_bg_color(0xF5F543);
                        break;
                    case COLOR_BLUE:
                        devconsole_set_bg_color(0x3B8EEA);
                        break;
                    case COLOR_PURPLE:
                        devconsole_set_bg_color(0xD670D6);
                        break;
                    case COLOR_CYAN:
                        devconsole_set_bg_color(0x29B8DB);
                        break;
                    case COLOR_WHITE:
                        devconsole_set_bg_color(0xFFFFFF);
                        break;

                    default:
                        break;
                }
                break;
            }

            default:
                break;
        }
    }

    return i;
}
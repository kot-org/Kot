#ifndef KUI_RENDERER_H
#define KUI_RENDERER_H

#include <kot-ui/core.h>


#if defined(__cplusplus)
extern "C" {
#endif

void kui_r_init();
void kui_r_create_window(kui_Context *ctx, kui_Container *cnt, kui_Rect rect);
void kui_r_move_window(kui_Container *cnt, kui_Vec2 pos);
void kui_r_resize_window(kui_Container *cnt, kui_Rect rect);
void kui_r_draw_rect(kui_Container *cnt, kui_Rect rect, kui_Color color);
void kui_r_draw_text(kui_Container *cnt, kui_Font font, const char *text, int len, kui_Vec2 pos, kui_Color color);
void kui_r_draw_icon(kui_Container *cnt, int id, kui_Rect rect, kui_Color color);
int kui_r_get_text_width(kui_Container *cnt, kui_Font font, const char *text, int len);
int kui_r_get_text_height(kui_Container *cnt, kui_Font font);
void kui_r_set_clip_rect(kui_Container *cnt, kui_Rect rect);
void kui_r_clear(kui_Container *cnt, kui_Color color);
void kui_r_present(kui_Container *cnt);


#if defined(__cplusplus)
}
#endif

#endif


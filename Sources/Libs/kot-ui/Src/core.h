/*
** Copyright (c) 2020 rxi
**
** This library is free software; you can redistribute it and/or modify it
** under the terms of the MIT license. See `core.c` for details.
*/

#ifndef KUI_CORE_H
#define KUI_CORE_H

#include <kot/types.h>
#include <kot-graphics/utils.h>


#if defined(__cplusplus)
extern "C" {
#endif

#define KUI_VERSION "2.01"

#define KUI_COMMANDLIST_SIZE     (256 * 1024)
#define KUI_ROOTLIST_SIZE        32
#define KUI_CONTAINERSTACK_SIZE  32
#define KUI_CLIPSTACK_SIZE       32
#define KUI_IDSTACK_SIZE         32
#define KUI_LAYOUTSTACK_SIZE     16
#define KUI_CONTAINERPOOL_SIZE   48
#define KUI_TREENODEPOOL_SIZE    48
#define KUI_MAX_WIDTHS           16
#define KUI_REAL                 float
#define KUI_REAL_FMT             "%.3g"
#define KUI_SLIDER_FMT           "%.2f"
#define KUI_MAX_FMT              127

#define kui_stack(T, n)          struct { int idx; T items[n]; }
#define kui_min(a, b)            ((a) < (b) ? (a) : (b))
#define kui_max(a, b)            ((a) > (b) ? (a) : (b))
#define kui_clamp(x, a, b)       kui_min(b, kui_max(a, x))

enum {
  KUI_CLIP_PART = 1,
  KUI_CLIP_ALL
};

enum {
  KUI_COMMAND_JUMP = 1,
  KUI_COMMAND_FRAMEBUFFER,
  KUI_COMMAND_CLIP,
  KUI_COMMAND_RECT,
  KUI_COMMAND_TEXT,
  KUI_COMMAND_ICON,
  KUI_COMMAND_MAX
};

enum {
  KUI_COLOR_TEXT,
  KUI_COLOR_BORDER,
  KUI_COLOR_WINDOWBG,
  KUI_COLOR_TITLEBG,
  KUI_COLOR_TITLETEXT,
  KUI_COLOR_PANELBG,
  KUI_COLOR_BUTTON,
  KUI_COLOR_BUTTONHOVER,
  KUI_COLOR_BUTTONFOCUS,
  KUI_COLOR_BUTTONDARK,
  KUI_COLOR_BUTTONDARKHOVER,
  KUI_COLOR_BUTTONDARKFOCUS,
  KUI_COLOR_BASE,
  KUI_COLOR_BASEHOVER,
  KUI_COLOR_BASEFOCUS,
  KUI_COLOR_THUMB,
  KUI_COLOR_THUMBHOVER,
  KUI_COLOR_THUMBFOCUS,
  KUI_COLOR_SCROLLBASE,
  KUI_COLOR_SCROLLTHUMB,
  KUI_COLOR_MAX
};

enum {
  KUI_ICON_CLOSE = 927,
  KUI_ICON_COLLAPSED = 929,
  KUI_ICON_EXPANDED = 928,
  KUI_ICON_CHECK = 921,
  KUI_ICON_MAX = 9160
};

enum {
  KUI_RES_ACTIVE       = (1 << 0),
  KUI_RES_SUBMIT       = (1 << 1),
  KUI_RES_CHANGE       = (1 << 2)
};

enum {
  KUI_OPT_ALIGNCENTER  = (1 << 0),
  KUI_OPT_ALIGNRIGHT   = (1 << 1),
  KUI_OPT_NOINTERACT   = (1 << 2),
  KUI_OPT_NOFRAME      = (1 << 3),
  KUI_OPT_NORESIZE     = (1 << 4),
  KUI_OPT_NOSCROLL     = (1 << 5),
  KUI_OPT_NOCLOSE      = (1 << 6),
  KUI_OPT_NOTITLE      = (1 << 7),
  KUI_OPT_HOLDFOCUS    = (1 << 8),
  KUI_OPT_AUTOSIZE     = (1 << 9),
  KUI_OPT_POPUP        = (1 << 10),
  KUI_OPT_CLOSED       = (1 << 11),
  KUI_OPT_EXPANDED     = (1 << 12),
  KUI_OPT_NOBORDER     = (1 << 13)
};

enum {
  KUI_MOUSE_LEFT       = (1 << 0),
  KUI_MOUSE_RIGHT      = (1 << 1),
  KUI_MOUSE_MIDDLE     = (1 << 2)
};

enum {
  KUI_KEY_SHIFT        = (1 << 0),
  KUI_KEY_CTRL         = (1 << 1),
  KUI_KEY_ALT          = (1 << 2),
  KUI_KEY_BACKSPACE    = (1 << 3),
  KUI_KEY_RETURN       = (1 << 4)
};


typedef struct kui_Context kui_Context;
typedef struct kui_Container kui_Container;
typedef kot_framebuffer_t kui_framebuffer_t;
typedef unsigned kui_Id;
typedef KUI_REAL kui_Real;
typedef void* kui_Font;

typedef struct { int x, y; } kui_Vec2;
typedef struct { int x, y, w, h; } kui_Rect;
typedef struct { unsigned char r, g, b, a; } kui_Color;
typedef struct { kui_Id id; int last_update; } kui_PoolItem;

typedef struct { int type, size; kui_Container *cnt; } kui_BaseCommand;
typedef struct { kui_BaseCommand base; void *dst; } kui_JumpCommand;
typedef struct { kui_BaseCommand base; kui_Rect rect; kui_framebuffer_t *fb; } kui_FramebufferCommand;
typedef struct { kui_BaseCommand base; kui_Rect rect; } kui_ClipCommand;
typedef struct { kui_BaseCommand base; kui_Rect rect; kui_Color color; } kui_RectCommand;
typedef struct { kui_BaseCommand base; kui_Font font; kui_Vec2 pos; kui_Color color; size_t len; char str[1]; } kui_TextCommand;
typedef struct { kui_BaseCommand base; kui_Rect rect; int id; kui_Color color; } kui_IconCommand;

typedef void (*kui_ProcessFrameCallback)(kui_Context* ctx);

typedef union {
  int type;
  kui_BaseCommand base;
  kui_JumpCommand jump;
  kui_FramebufferCommand framebuffer;
  kui_ClipCommand clip;
  kui_RectCommand rect;
  kui_TextCommand text;
  kui_IconCommand icon;
} kui_Command;

typedef struct {
  kui_Rect body;
  kui_Rect next;
  kui_Vec2 position;
  kui_Vec2 size;
  kui_Vec2 max;
  int widths[KUI_MAX_WIDTHS];
  int items;
  int item_index;
  int next_row;
  int next_type;
  int indent;
} kui_Layout;

typedef struct {
  bool is_window;
  kui_framebuffer_t backbuffer;
  kot_window_t *window;
  kot_thread_t window_handler_thread;
  kot_event_t window_event;
  kui_Color last_color;
  kui_Context *ctx;
  kui_Font default_font;
  kui_Font icons_font;
} kui_Window;

typedef struct {
  kui_Font font;
  kui_Vec2 size;
  int padding;
  int spacing;
  int indent;
  int title_height;
  int scrollbar_size;
  int thumb_size;
  kui_Color colors[KUI_COLOR_MAX];
} kui_Style;

struct kui_Container {
  kui_Command *head, *tail;
  kui_Rect rect;
  kui_Rect body;
  kui_Vec2 content_size;
  kui_Vec2 scroll;
  int zindex;
  int open;
  /* kot specific data*/
  kui_Window* window_parent;
  bool is_windows;
};

struct kui_Context {
  /* core state */
  kui_Style _style;
  kui_Style *style;
  kui_Id hover;
  kui_Id focus;
  kui_Id last_id;
  kui_Rect last_rect;
  int last_zindex;
  int updated_focus;
  int frame;
  kui_Container *hover_root;
  kui_Container *next_hover_root;
  kui_Container *scroll_target;
  char number_edit_buf[KUI_MAX_FMT];
  kui_Id number_edit;
  /* stacks */
  kui_stack(char, KUI_COMMANDLIST_SIZE) command_list;
  kui_stack(kui_Container*, KUI_ROOTLIST_SIZE) root_list;
  kui_stack(kui_Container*, KUI_CONTAINERSTACK_SIZE) container_stack;
  kui_stack(kui_Rect, KUI_CLIPSTACK_SIZE) clip_stack;
  kui_stack(kui_Id, KUI_IDSTACK_SIZE) id_stack;
  kui_stack(kui_Layout, KUI_LAYOUTSTACK_SIZE) layout_stack;
  /* retained state pools */
  kui_PoolItem container_pool[KUI_CONTAINERPOOL_SIZE];
  kui_Container containers[KUI_CONTAINERPOOL_SIZE];
  kui_PoolItem treenode_pool[KUI_TREENODEPOOL_SIZE];
  /* input state */
  kui_Vec2 mouse_pos;
  kui_Vec2 last_mouse_pos;
  kui_Vec2 mouse_delta;
  kui_Vec2 scroll_delta;
  int mouse_down;
  int mouse_pressed;
  int key_down;
  int key_pressed;
  char input_text[32];
  /* kot specific data */
  uint64_t last_mouse_status;
  kui_ProcessFrameCallback callback_frame;
  void* opaque;
};


kui_Vec2 kui_vec2(int x, int y);
kui_Rect kui_rect(int x, int y, int w, int h);
kui_Color kui_color(int r, int g, int b, int a);

kui_Context* kui_init(kui_ProcessFrameCallback callback, void* opaque);
void kui_begin(kui_Context *ctx);
void kui_end(kui_Context *ctx);
void kui_set_focus(kui_Context *ctx, kui_Id id);
kui_Id kui_get_id(kui_Context *ctx, const void *data, int size);
void kui_push_id(kui_Context *ctx, const void *data, int size);
void kui_pop_id(kui_Context *ctx);
void kui_push_clip_rect(kui_Context *ctx, kui_Rect rect);
void kui_pop_clip_rect(kui_Context *ctx);
kui_Rect kui_get_clip_rect(kui_Context *ctx);
int kui_check_clip(kui_Context *ctx, kui_Rect r);
kui_Container* kui_get_current_container(kui_Context *ctx);
kui_Container* kui_get_container(kui_Context *ctx, const char *name);
void kui_bring_to_front(kui_Context *ctx, kui_Container *cnt);

int kui_pool_init(kui_Context *ctx, kui_PoolItem *items, int len, kui_Id id);
int kui_pool_get(kui_Context *ctx, kui_PoolItem *items, int len, kui_Id id);
void kui_pool_update(kui_Context *ctx, kui_PoolItem *items, int idx);

void kui_input_mousemove(kui_Context *ctx, int x, int y);
void kui_input_mousedown(kui_Context *ctx, int x, int y, int btn);
void kui_input_mouseup(kui_Context *ctx, int x, int y, int btn);
void kui_input_scroll(kui_Context *ctx, int x, int y);
void kui_input_keydown(kui_Context *ctx, int key);
void kui_input_keyup(kui_Context *ctx, int key);
void kui_input_text(kui_Context *ctx, const char *text);

kui_Command* kui_push_command(kui_Context *ctx, int type, int size);
int kui_next_command(kui_Context *ctx, kui_Command **cmd);
void kui_set_framebuffer(kui_Context *ctx, kui_Rect rect, kui_framebuffer_t* fb);
void kui_set_clip(kui_Context *ctx, kui_Rect rect);
void kui_draw_rect(kui_Context *ctx, kui_Rect rect, kui_Color color);
void kui_draw_box(kui_Context *ctx, kui_Rect rect, kui_Color color);
void kui_draw_text(kui_Context *ctx, kui_Font font, const char *str, int len, kui_Vec2 pos, kui_Color color);
void kui_draw_icon(kui_Context *ctx, int id, kui_Rect rect, kui_Color color);

void kui_layout_row(kui_Context *ctx, int items, const int *widths, int height);
void kui_layout_width(kui_Context *ctx, int width);
void kui_layout_height(kui_Context *ctx, int height);
void kui_layout_begin_column(kui_Context *ctx);
void kui_layout_end_column(kui_Context *ctx);
void kui_layout_set_next(kui_Context *ctx, kui_Rect r, int relative);
kui_Rect kui_layout_next(kui_Context *ctx);

void kui_draw_control_frame(kui_Context *ctx, kui_Id id, kui_Rect rect, int colorid, int opt);
void kui_draw_control_text(kui_Context *ctx, const char *str, kui_Rect rect, int colorid, int opt);
int kui_mouse_over(kui_Context *ctx, kui_Rect rect);
void kui_update_control(kui_Context *ctx, kui_Id id, kui_Rect rect, int opt);

#define kui_button(ctx, label)                  kui_button_ex(ctx, label, 0, KUI_OPT_ALIGNCENTER)
#define kui_textbox(ctx, buf, bufsz)            kui_textbox_ex(ctx, buf, bufsz, 0)
#define kui_slider(ctx, value, lo, hi)          kui_slider_ex(ctx, value, lo, hi, 0, KUI_SLIDER_FMT, KUI_OPT_ALIGNCENTER)
#define kui_number(ctx, value, step)            kui_number_ex(ctx, value, step, KUI_SLIDER_FMT, KUI_OPT_ALIGNCENTER)
#define kui_header(ctx, label)                  kui_header_ex(ctx, label, 0)
#define kui_begin_treenode(ctx, label)          kui_begin_treenode_ex(ctx, label, 0)
#define kui_begin_window(ctx, title, rect)      kui_begin_window_ex(ctx, title, rect, 0)
#define kui_begin_panel(ctx, name)              kui_begin_panel_ex(ctx, name, 0)

void kui_text(kui_Context *ctx, const char *text);
void kui_label(kui_Context *ctx, const char *text);
int kui_button_ex(kui_Context *ctx, const char *label, int icon, int opt);
int kui_checkbox(kui_Context *ctx, const char *label, int *state);
int kui_textbox_raw(kui_Context *ctx, char *buf, int bufsz, kui_Id id, kui_Rect r, int opt);
int kui_textbox_ex(kui_Context *ctx, char *buf, int bufsz, int opt);
int kui_slider_ex(kui_Context *ctx, kui_Real *value, kui_Real low, kui_Real high, kui_Real step, const char *fmt, int opt);
int kui_number_ex(kui_Context *ctx, kui_Real *value, kui_Real step, const char *fmt, int opt);
int kui_header_ex(kui_Context *ctx, const char *label, int opt);
int kui_begin_treenode_ex(kui_Context *ctx, const char *label, int opt);
void kui_end_treenode(kui_Context *ctx);
int kui_begin_window_ex(kui_Context *ctx, const char *title, kui_Rect rect, int opt);
void kui_end_window(kui_Context *ctx);
void kui_open_popup(kui_Context *ctx, const char *name);
int kui_begin_popup(kui_Context *ctx, const char *name);
void kui_end_popup(kui_Context *ctx);
void kui_begin_panel_ex(kui_Context *ctx, const char *name, int opt);
void kui_end_panel(kui_Context *ctx);

static inline color_t kui_convert_color_to_kot_color(kui_Color color){
  return color.b | (color.g << 8) | (color.r << 16) | (color.a << 24);
}

#if defined(__cplusplus)
}
#endif

#endif

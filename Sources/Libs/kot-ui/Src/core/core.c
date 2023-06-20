/*
** Copyright (c) 2020 rxi
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to
** deal in the Software without restriction, including without limitation the
** rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
** sell copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
** IN THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kot-ui/core.h>

#define unused(x) ((void) (x))

#define expect(x) do {                                               \
    if (!(x)) {                                                      \
      fprintf(stderr, "Fatal error: %s:%d: assertion '%s' failed\n", \
        __FILE__, __LINE__, #x);                                     \
      abort();                                                       \
    }                                                                \
  } while (0)

#define push(stk, val) do {                                                 \
    expect((stk).idx < (int) (sizeof((stk).items) / sizeof(*(stk).items))); \
    (stk).items[(stk).idx] = (val);                                         \
    (stk).idx++; /* incremented after incase `val` uses this value */       \
  } while (0)

#define pop(stk) do {      \
    expect((stk).idx > 0); \
    (stk).idx--;           \
  } while (0)


static kui_Rect unclipped_rect = { 0, 0, 0x1000000, 0x1000000 };

static kui_Style default_style = {
  /* font | size | padding | spacing | indent */
  NULL, { 68, 10 }, 5, 4, 24,
  /* title_height | scrollbar_size | thumb_size */
  24, 12, 8,
  {
    { 230, 230, 230, 255 }, /* KUI_COLOR_TEXT */
    { 25,  24,  26,  255 }, /* KUI_COLOR_BORDER */
    { 9,   9,   11,  255 }, /* KUI_COLOR_WINDOWBG */
    { 25,  24,  26,  255 }, /* KUI_COLOR_TITLEBG */
    { 240, 240, 240, 255 }, /* KUI_COLOR_TITLETEXT */
    { 0,   0,   0,   0   }, /* KUI_COLOR_PANELBG */
    { 25,  24,  26,  255 }, /* KUI_COLOR_BUTTON */
    { 51,  153, 255, 255 }, /* KUI_COLOR_BUTTONHOVER */
    { 12,  75,  221, 255 }, /* KUI_COLOR_BUTTONFOCUS */
    { 30,  30,  30,  255 }, /* KUI_COLOR_BASE */
    { 35,  35,  35,  255 }, /* KUI_COLOR_BASEHOVER */
    { 40,  40,  40,  255 }, /* KUI_COLOR_BASEFOCUS */
    { 30,  30,  30,  255 }, /* KUI_COLOR_SCROLLBASE */
    { 43,  43,  43,  255 }, /* KUI_COLOR_SCROLLTHUMB */
  }
};


kui_Vec2 kui_vec2(int x, int y) {
  kui_Vec2 res;
  res.x = x; res.y = y;
  return res;
}


kui_Rect kui_rect(int x, int y, int w, int h) {
  kui_Rect res;
  res.x = x; res.y = y; res.w = w; res.h = h;
  return res;
}


kui_Color kui_color(int r, int g, int b, int a) {
  kui_Color res;
  res.r = r; res.g = g; res.b = b; res.a = a;
  return res;
}


static kui_Rect expand_rect(kui_Rect rect, int n) {
  return kui_rect(rect.x - n, rect.y - n, rect.w + n * 2, rect.h + n * 2);
}


static kui_Rect intersect_rects(kui_Rect r1, kui_Rect r2) {
  int x1 = kui_max(r1.x, r2.x);
  int y1 = kui_max(r1.y, r2.y);
  int x2 = kui_min(r1.x + r1.w, r2.x + r2.w);
  int y2 = kui_min(r1.y + r1.h, r2.y + r2.h);
  if (x2 < x1) { x2 = x1; }
  if (y2 < y1) { y2 = y1; }
  return kui_rect(x1, y1, x2 - x1, y2 - y1);
}


static int rect_overlaps_vec2(kui_Rect r, kui_Vec2 p) {
  return p.x >= r.x && p.x < r.x + r.w && p.y >= r.y && p.y < r.y + r.h;
}


static void draw_frame(kui_Context *ctx, kui_Rect rect, int colorid) {
  kui_draw_rect(ctx, rect, ctx->style->colors[colorid]);
  if (colorid == KUI_COLOR_SCROLLBASE  ||
      colorid == KUI_COLOR_SCROLLTHUMB ||
      colorid == KUI_COLOR_TITLEBG) { return; }
  /* draw border */
  if (ctx->style->colors[KUI_COLOR_BORDER].a) {
    kui_draw_box(ctx, expand_rect(rect, 1), ctx->style->colors[KUI_COLOR_BORDER]);
  }
}


kui_Context* kui_init(kui_ProcessFrameCallback callback, void* opaque) {
  kui_Context *ctx = calloc(1, sizeof(kui_Context));
  ctx->_style = default_style;
  ctx->style = &ctx->_style;
  ctx->callback_frame = callback;
  ctx->opaque = opaque;
  kui_r_init();
  ctx->callback_frame(ctx);  
  ctx->callback_frame(ctx);
  return ctx;
}


void kui_begin(kui_Context *ctx) {
  ctx->command_list.idx = 0;
  ctx->root_list.idx = 0;
  ctx->scroll_target = NULL;
  ctx->hover_root = ctx->next_hover_root;
  ctx->next_hover_root = NULL;
  ctx->mouse_delta.x = ctx->mouse_pos.x - ctx->last_mouse_pos.x;
  ctx->mouse_delta.y = ctx->mouse_pos.y - ctx->last_mouse_pos.y;
  ctx->frame++;
}


static int compare_zindex(const void *a, const void *b) {
  return (*(kui_Container**) a)->zindex - (*(kui_Container**) b)->zindex;
}


void kui_end(kui_Context *ctx) {
  int i, n;
  /* check stacks */
  expect(ctx->container_stack.idx == 0);
  expect(ctx->clip_stack.idx      == 0);
  expect(ctx->id_stack.idx        == 0);
  expect(ctx->layout_stack.idx    == 0);

  /* handle scroll input */
  if (ctx->scroll_target) {
    ctx->scroll_target->scroll.x += ctx->scroll_delta.x;
    ctx->scroll_target->scroll.y += ctx->scroll_delta.y;
  }

  /* unset focus if focus id was not touched this frame */
  if (!ctx->updated_focus) { ctx->focus = 0; }
  ctx->updated_focus = 0;

  /* bring hover root to front if mouse was pressed */
  if (ctx->mouse_pressed && ctx->next_hover_root &&
      ctx->next_hover_root->zindex < ctx->last_zindex &&
      ctx->next_hover_root->zindex >= 0
  ) {
    kui_bring_to_front(ctx, ctx->next_hover_root);
  }

  /* reset input state */
  ctx->key_pressed = 0;
  ctx->input_text[0] = '\0';
  ctx->mouse_pressed = 0;
  ctx->scroll_delta = kui_vec2(0, 0);
  ctx->last_mouse_pos = ctx->mouse_pos;

  /* sort root containers by zindex */
  n = ctx->root_list.idx;
  qsort(ctx->root_list.items, n, sizeof(kui_Container*), compare_zindex);

  /* set root container jump commands */
  for (i = 0; i < n; i++) {
    kui_Container *cnt = ctx->root_list.items[i];
    /* if this is the first container then make the first command jump to it.
    ** otherwise set the previous container's tail to jump to this one */
    if (i == 0) {
      kui_Command *cmd = (kui_Command*) ctx->command_list.items;
      cmd->jump.dst = (char*) cnt->head + sizeof(kui_JumpCommand);
    } else {
      kui_Container *prev = ctx->root_list.items[i - 1];
      prev->tail->jump.dst = (char*) cnt->head + sizeof(kui_JumpCommand);
    }
    /* make the last container's tail jump to the end of command list */
    if (i == n - 1) {
      cnt->tail->jump.dst = ctx->command_list.items + ctx->command_list.idx;
    }
  }

  /* render everything */
  kui_Command *cmd = NULL;
  while (kui_next_command(ctx, &cmd)) {
    if (cmd->type == KUI_COMMAND_TEXT) {
      kui_r_draw_text(cmd->base.cnt, cmd->text.font, cmd->text.str, cmd->text.len, cmd->text.pos, cmd->text.color);
    }
    if (cmd->type == KUI_COMMAND_RECT) {
      kui_r_draw_rect(cmd->base.cnt, cmd->rect.rect, cmd->rect.color);
    }
    if (cmd->type == KUI_COMMAND_ICON) {
      kui_r_draw_icon(cmd->base.cnt, cmd->icon.id, cmd->icon.rect, cmd->icon.color);
    }
    if (cmd->type == KUI_COMMAND_CLIP) {
      kui_r_set_clip_rect(cmd->base.cnt, cmd->clip.rect);
    }
    if (cmd->type == KUI_COMMAND_FRAMEBUFFER){
      kui_r_framebuffer(cmd->base.cnt, cmd->framebuffer.fb, cmd->framebuffer.rect);
    }
  }
}


void kui_set_focus(kui_Context *ctx, kui_Id id) {
  ctx->focus = id;
  ctx->updated_focus = 1;
}


/* 32bit fnv-1a hash */
#define HASH_INITIAL 2166136261

static void hash(kui_Id *hash, const void *data, int size) {
  const unsigned char *p = data;
  while (size--) {
    *hash = (*hash ^ *p++) * 16777619;
  }
}


kui_Id kui_get_id(kui_Context *ctx, const void *data, int size) {
  int idx = ctx->id_stack.idx;
  kui_Id res = (idx > 0) ? ctx->id_stack.items[idx - 1] : HASH_INITIAL;
  hash(&res, data, size);
  ctx->last_id = res;
  return res;
}


void kui_push_id(kui_Context *ctx, const void *data, int size) {
  push(ctx->id_stack, kui_get_id(ctx, data, size));
}


void kui_pop_id(kui_Context *ctx) {
  pop(ctx->id_stack);
}


void kui_push_clip_rect(kui_Context *ctx, kui_Rect rect) {
  kui_Rect last = kui_get_clip_rect(ctx);
  push(ctx->clip_stack, intersect_rects(rect, last));
}


void kui_pop_clip_rect(kui_Context *ctx) {
  pop(ctx->clip_stack);
}


kui_Rect kui_get_clip_rect(kui_Context *ctx) {
  expect(ctx->clip_stack.idx > 0);
  return ctx->clip_stack.items[ctx->clip_stack.idx - 1];
}


int kui_check_clip(kui_Context *ctx, kui_Rect r) {
  kui_Rect cr = kui_get_clip_rect(ctx);
  if (r.x > cr.x + cr.w || r.x + r.w < cr.x ||
      r.y > cr.y + cr.h || r.y + r.h < cr.y   ) { return KUI_CLIP_ALL; }
  if (r.x >= cr.x && r.x + r.w <= cr.x + cr.w &&
      r.y >= cr.y && r.y + r.h <= cr.y + cr.h ) { return 0; }
  return KUI_CLIP_PART;
}


static void push_layout(kui_Context *ctx, kui_Rect body, kui_Vec2 scroll) {
  kui_Layout layout;
  int width = 0;
  memset(&layout, 0, sizeof(layout));
  layout.body = kui_rect(body.x - scroll.x, body.y - scroll.y, body.w, body.h);
  layout.max = kui_vec2(-0x1000000, -0x1000000);
  push(ctx->layout_stack, layout);
  kui_layout_row(ctx, 1, &width, 0);
}


static kui_Layout* get_layout(kui_Context *ctx) {
  return &ctx->layout_stack.items[ctx->layout_stack.idx - 1];
}


static void pop_container(kui_Context *ctx) {
  kui_Container *cnt = kui_get_current_container(ctx);
  kui_Layout *layout = get_layout(ctx);
  cnt->content_size.x = layout->max.x - layout->body.x;
  cnt->content_size.y = layout->max.y - layout->body.y;
  /* pop container, layout and id */
  pop(ctx->container_stack);
  pop(ctx->layout_stack);
  kui_pop_id(ctx);
}


kui_Container* kui_get_current_container(kui_Context *ctx) {
  expect(ctx->container_stack.idx > 0);
  return ctx->container_stack.items[ ctx->container_stack.idx - 1 ];
}


static kui_Container* get_container(kui_Context *ctx, kui_Id id, int opt) {
  kui_Container *cnt;
  kui_Window* Parent;
  if(ctx->container_stack.idx > 0){
    Parent = kui_get_current_container(ctx)->window_parent;
  }else{
    Parent = NULL;
  }
  /* try to get existing container from pool */
  int idx = kui_pool_get(ctx, ctx->container_pool, KUI_CONTAINERPOOL_SIZE, id);
  if (idx >= 0) {
    if (ctx->containers[idx].open || ~opt & KUI_OPT_CLOSED) {
      kui_pool_update(ctx, ctx->container_pool, idx);
    }
    if(Parent){
      ctx->containers[idx].window_parent = Parent;
    }
    return &ctx->containers[idx];
  }
  if (opt & KUI_OPT_CLOSED) { return NULL; }
  /* container not found in pool: init new container */
  idx = kui_pool_init(ctx, ctx->container_pool, KUI_CONTAINERPOOL_SIZE, id);
  cnt = &ctx->containers[idx];
  memset(cnt, 0, sizeof(*cnt));
  cnt->open = 1;
  kui_bring_to_front(ctx, cnt);
  if(Parent){
    cnt->window_parent = Parent;
  }
  return cnt;
}


kui_Container* kui_get_container(kui_Context *ctx, const char *name) {
  kui_Id id = kui_get_id(ctx, name, strlen(name));
  return get_container(ctx, id, 0);
}


void kui_bring_to_front(kui_Context *ctx, kui_Container *cnt) {
  cnt->zindex = ++ctx->last_zindex;
}


/*============================================================================
** pool
**============================================================================*/

int kui_pool_init(kui_Context *ctx, kui_PoolItem *items, int len, kui_Id id) {
  int i, n = -1, f = ctx->frame;
  for (i = 0; i < len; i++) {
    if (items[i].last_update < f) {
      f = items[i].last_update;
      n = i;
    }
  }
  expect(n > -1);
  items[n].id = id;
  kui_pool_update(ctx, items, n);
  return n;
}


int kui_pool_get(kui_Context *ctx, kui_PoolItem *items, int len, kui_Id id) {
  int i;
  unused(ctx);
  for (i = 0; i < len; i++) {
    if (items[i].id == id) { return i; }
  }
  return -1;
}


void kui_pool_update(kui_Context *ctx, kui_PoolItem *items, int idx) {
  items[idx].last_update = ctx->frame;
}


/*============================================================================
** input handlers
**============================================================================*/

void kui_input_mousemove(kui_Context *ctx, int x, int y) {
  ctx->mouse_pos = kui_vec2(x, y);
}


void kui_input_mousedown(kui_Context *ctx, int x, int y, int btn) {
  kui_input_mousemove(ctx, x, y);
  ctx->mouse_down |= btn;
  ctx->mouse_pressed |= btn;
}


void kui_input_mouseup(kui_Context *ctx, int x, int y, int btn) {
  kui_input_mousemove(ctx, x, y);
  ctx->mouse_down &= ~btn;
}


void kui_input_scroll(kui_Context *ctx, int x, int y) {
  ctx->scroll_delta.x += x;
  ctx->scroll_delta.y += y;
}


void kui_input_keydown(kui_Context *ctx, int key) {
  ctx->key_pressed |= key;
  ctx->key_down |= key;
}


void kui_input_keyup(kui_Context *ctx, int key) {
  ctx->key_down &= ~key;
}


void kui_input_text(kui_Context *ctx, const char *text) {
  int len = strlen(ctx->input_text);
  int size = strlen(text) + 1;
  expect(len + size <= (int) sizeof(ctx->input_text));
  memcpy(ctx->input_text + len, text, size);
}


/*============================================================================
** commandlist
**============================================================================*/

kui_Command* kui_push_command(kui_Context *ctx, int type, int size) {
  kui_Command *cmd = (kui_Command*) (ctx->command_list.items + ctx->command_list.idx);
  expect(ctx->command_list.idx + size < KUI_COMMANDLIST_SIZE);
  cmd->base.type = type;
  cmd->base.size = size;
  cmd->base.cnt = kui_get_current_container(ctx);
  ctx->command_list.idx += size;
  return cmd;
}


int kui_next_command(kui_Context *ctx, kui_Command **cmd) {
  if (*cmd) {
    *cmd = (kui_Command*) (((char*) *cmd) + (*cmd)->base.size);
  } else {
    *cmd = (kui_Command*) ctx->command_list.items;
  }
  while ((char*) *cmd != ctx->command_list.items + ctx->command_list.idx) {
    if ((*cmd)->type != KUI_COMMAND_JUMP) { return 1; }
    *cmd = (*cmd)->jump.dst;
  }
  return 0;
}


static kui_Command* push_jump(kui_Context *ctx, kui_Command *dst) {
  kui_Command *cmd;
  cmd = kui_push_command(ctx, KUI_COMMAND_JUMP, sizeof(kui_JumpCommand));
  cmd->jump.dst = dst;
  return cmd;
}


void kui_set_framebuffer(kui_Context *ctx, kui_Rect rect, kui_framebuffer_t* fb) {
  kui_Command *cmd;
  cmd = kui_push_command(ctx, KUI_COMMAND_FRAMEBUFFER, sizeof(kui_FramebufferCommand));
  cmd->framebuffer.rect = rect;
  cmd->framebuffer.fb = fb;
}

void kui_set_clip(kui_Context *ctx, kui_Rect rect) {
  kui_Command *cmd;
  cmd = kui_push_command(ctx, KUI_COMMAND_CLIP, sizeof(kui_ClipCommand));
  cmd->clip.rect = rect;
}


void kui_draw_rect(kui_Context *ctx, kui_Rect rect, kui_Color color) {
  kui_Command *cmd;
  rect = intersect_rects(rect, kui_get_clip_rect(ctx));
  if (rect.w > 0 && rect.h > 0) {
    cmd = kui_push_command(ctx, KUI_COMMAND_RECT, sizeof(kui_RectCommand));
    cmd->rect.rect = rect;
    cmd->rect.color = color;
  }
}


void kui_draw_box(kui_Context *ctx, kui_Rect rect, kui_Color color) {
  kui_draw_rect(ctx, kui_rect(rect.x + 1, rect.y, rect.w - 2, 1), color);
  kui_draw_rect(ctx, kui_rect(rect.x + 1, rect.y + rect.h - 1, rect.w - 2, 1), color);
  kui_draw_rect(ctx, kui_rect(rect.x, rect.y, 1, rect.h), color);
  kui_draw_rect(ctx, kui_rect(rect.x + rect.w - 1, rect.y, 1, rect.h), color);
}


void kui_draw_text(kui_Context *ctx, kui_Font font, const char *str, int len,
  kui_Vec2 pos, kui_Color color)
{
  kui_Command *cmd;
  kui_Rect rect = kui_rect(
    pos.x, pos.y, kui_r_get_text_width(kui_get_current_container(ctx), font, str, len), kui_r_get_text_height(kui_get_current_container(ctx), font));
  int clipped = kui_check_clip(ctx, rect);
  if (clipped == KUI_CLIP_ALL ) { return; }
  if (clipped == KUI_CLIP_PART) { kui_set_clip(ctx, kui_get_clip_rect(ctx)); }
  /* add command */
  if (len < 0) { len = strlen(str); }
  cmd = kui_push_command(ctx, KUI_COMMAND_TEXT, sizeof(kui_TextCommand) + len);
  memcpy(cmd->text.str, str, len);
  cmd->text.str[len] = '\0';
  cmd->text.len = len;
  cmd->text.pos = pos;
  cmd->text.color = color;
  cmd->text.font = font;
  /* reset clipping if it was set */
  if (clipped) { kui_set_clip(ctx, unclipped_rect); }
}


void kui_draw_icon(kui_Context *ctx, int id, kui_Rect rect, kui_Color color) {
  kui_Command *cmd;
  /* do clip command if the rect isn't fully contained within the cliprect */
  int clipped = kui_check_clip(ctx, rect);
  if (clipped == KUI_CLIP_ALL ) { return; }
  if (clipped == KUI_CLIP_PART) { kui_set_clip(ctx, kui_get_clip_rect(ctx)); }
  /* do icon command */
  cmd = kui_push_command(ctx, KUI_COMMAND_ICON, sizeof(kui_IconCommand));
  cmd->icon.id = id;
  cmd->icon.rect = rect;
  cmd->icon.color = color;
  /* reset clipping if it was set */
  if (clipped) { kui_set_clip(ctx, unclipped_rect); }
}


/*============================================================================
** layout
**============================================================================*/

enum { RELATIVE = 1, ABSOLUTE = 2 };


void kui_layout_begin_column(kui_Context *ctx) {
  push_layout(ctx, kui_layout_next(ctx), kui_vec2(0, 0));
}


void kui_layout_end_column(kui_Context *ctx) {
  kui_Layout *a, *b;
  b = get_layout(ctx);
  pop(ctx->layout_stack);
  /* inherit position/next_row/max from child layout if they are greater */
  a = get_layout(ctx);
  a->position.x = kui_max(a->position.x, b->position.x + b->body.x - a->body.x);
  a->next_row = kui_max(a->next_row, b->next_row + b->body.y - a->body.y);
  a->max.x = kui_max(a->max.x, b->max.x);
  a->max.y = kui_max(a->max.y, b->max.y);
}


void kui_layout_row(kui_Context *ctx, int items, const int *widths, int height) {
  kui_Layout *layout = get_layout(ctx);
  if (widths) {
    expect(items <= KUI_MAX_WIDTHS);
    memcpy(layout->widths, widths, items * sizeof(widths[0]));
  }
  layout->items = items;
  layout->position = kui_vec2(layout->indent, layout->next_row);
  layout->size.y = height;
  layout->item_index = 0;
}


void kui_layout_width(kui_Context *ctx, int width) {
  get_layout(ctx)->size.x = width;
}


void kui_layout_height(kui_Context *ctx, int height) {
  get_layout(ctx)->size.y = height;
}


void kui_layout_set_next(kui_Context *ctx, kui_Rect r, int relative) {
  kui_Layout *layout = get_layout(ctx);
  layout->next = r;
  layout->next_type = relative ? RELATIVE : ABSOLUTE;
}


kui_Rect kui_layout_next(kui_Context *ctx) {
  kui_Layout *layout = get_layout(ctx);
  kui_Style *style = ctx->style;
  kui_Rect res;

  if (layout->next_type) {
    /* handle rect set by `kui_layout_set_next` */
    int type = layout->next_type;
    layout->next_type = 0;
    res = layout->next;
    if (type == ABSOLUTE) { return (ctx->last_rect = res); }

  } else {
    /* handle next row */
    if (layout->item_index == layout->items) {
      kui_layout_row(ctx, layout->items, NULL, layout->size.y);
    }

    /* position */
    res.x = layout->position.x;
    res.y = layout->position.y;

    /* size */
    res.w = layout->items > 0 ? layout->widths[layout->item_index] : layout->size.x;
    res.h = layout->size.y;
    if (res.w == 0) { res.w = style->size.x + style->padding * 2; }
    if (res.h == 0) { res.h = style->size.y + style->padding * 2; }
    if (res.w <  0) { res.w += layout->body.w - res.x + 1; }
    if (res.h <  0) { res.h += layout->body.h - res.y + 1; }

    layout->item_index++;
  }

  /* update position */
  layout->position.x += res.w + style->spacing;
  layout->next_row = kui_max(layout->next_row, res.y + res.h + style->spacing);

  /* apply body offset */
  res.x += layout->body.x;
  res.y += layout->body.y;

  /* update max position */
  layout->max.x = kui_max(layout->max.x, res.x + res.w);
  layout->max.y = kui_max(layout->max.y, res.y + res.h);

  return (ctx->last_rect = res);
}


/*============================================================================
** controls
**============================================================================*/

static int in_hover_root(kui_Context *ctx) {
  int i = ctx->container_stack.idx;
  while (i--) {
    if (ctx->container_stack.items[i] == ctx->hover_root) { return 1; }
    /* only root containers have their `head` field set; stop searching if we've
    ** reached the current root container */
    if (ctx->container_stack.items[i]->head) { break; }
  }
  return 0;
}


void kui_draw_control_frame(kui_Context *ctx, kui_Id id, kui_Rect rect,
  int colorid, int opt)
{
  if (opt & KUI_OPT_NOFRAME) { return; }
  colorid += (ctx->focus == id) ? 2 : (ctx->hover == id) ? 1 : 0;
  draw_frame(ctx, rect, colorid);
}


void kui_draw_control_text(kui_Context *ctx, const char *str, kui_Rect rect,
  int colorid, int opt)
{
  kui_Vec2 pos;
  kui_Font font = ctx->style->font;
  int tw = kui_r_get_text_width(kui_get_current_container(ctx), font, str, -1);
  kui_push_clip_rect(ctx, rect);
  pos.y = rect.y + (rect.h - kui_r_get_text_height(kui_get_current_container(ctx), font)) / 2;
  if (opt & KUI_OPT_ALIGNCENTER) {
    pos.x = rect.x + (rect.w - tw) / 2;
  } else if (opt & KUI_OPT_ALIGNRIGHT) {
    pos.x = rect.x + rect.w - tw - ctx->style->padding;
  } else {
    pos.x = rect.x + ctx->style->padding;
  }
  kui_draw_text(ctx, font, str, -1, pos, ctx->style->colors[colorid]);
  kui_pop_clip_rect(ctx);
}


int kui_mouse_over(kui_Context *ctx, kui_Rect rect) {
  return rect_overlaps_vec2(rect, ctx->mouse_pos) &&
    rect_overlaps_vec2(kui_get_clip_rect(ctx), ctx->mouse_pos) &&
    in_hover_root(ctx);
}


void kui_update_control(kui_Context *ctx, kui_Id id, kui_Rect rect, int opt) {
  int mouseover = kui_mouse_over(ctx, rect);

  if (ctx->focus == id) { ctx->updated_focus = 1; }
  if (opt & KUI_OPT_NOINTERACT) { return; }
  if (mouseover && !ctx->mouse_down) { ctx->hover = id; }

  if (ctx->focus == id) {
    if (ctx->mouse_pressed && !mouseover) { kui_set_focus(ctx, 0); }
    if (!ctx->mouse_down && ~opt & KUI_OPT_HOLDFOCUS) { kui_set_focus(ctx, 0); }
  }

  if (ctx->hover == id) {
    if (ctx->mouse_pressed) {
      kui_set_focus(ctx, id);
    } else if (!mouseover) {
      ctx->hover = 0;
    }
  }
}


void kui_text(kui_Context *ctx, const char *text) {
  const char *start, *end, *p = text;
  int width = -1;
  kui_Font font = ctx->style->font;
  kui_Color color = ctx->style->colors[KUI_COLOR_TEXT];
  kui_layout_begin_column(ctx);
  kui_layout_row(ctx, 1, &width, kui_r_get_text_height(kui_get_current_container(ctx), font));
  do {
    kui_Rect r = kui_layout_next(ctx);
    int w = 0;
    start = end = p;
    do {
      const char* word = p;
      while (*p && *p != ' ' && *p != '\n') { p++; }
      w += kui_r_get_text_width(kui_get_current_container(ctx), font, word, p - word);
      if (w > r.w && end != start) { break; }
      w += kui_r_get_text_width(kui_get_current_container(ctx), font, p, 1);
      end = p++;
    } while (*end && *end != '\n');
    kui_draw_text(ctx, font, start, end - start, kui_vec2(r.x, r.y), color);
    p = end + 1;
  } while (*end);
  kui_layout_end_column(ctx);
}


void kui_label(kui_Context *ctx, const char *text) {
  kui_draw_control_text(ctx, text, kui_layout_next(ctx), KUI_COLOR_TEXT, 0);
}


int kui_button_ex(kui_Context *ctx, const char *label, int icon, int opt) {
  int res = 0;
  kui_Id id = label ? kui_get_id(ctx, label, strlen(label))
                   : kui_get_id(ctx, &icon, sizeof(icon));
  kui_Rect r = kui_layout_next(ctx);
  kui_update_control(ctx, id, r, opt);
  /* handle click */
  if (ctx->mouse_pressed == KUI_MOUSE_LEFT && ctx->focus == id) {
    res |= KUI_RES_SUBMIT;
  }
  /* draw */
  kui_draw_control_frame(ctx, id, r, KUI_COLOR_BUTTON, opt);
  if (label) { kui_draw_control_text(ctx, label, r, KUI_COLOR_TEXT, opt); }
  if (icon) { kui_draw_icon(ctx, icon, r, ctx->style->colors[KUI_COLOR_TEXT]); }
  return res;
}


int kui_checkbox(kui_Context *ctx, const char *label, int *state) {
  int res = 0;
  kui_Id id = kui_get_id(ctx, &state, sizeof(state));
  kui_Rect r = kui_layout_next(ctx);
  kui_Rect box = kui_rect(r.x, r.y, r.h, r.h);
  kui_update_control(ctx, id, r, 0);
  /* handle click */
  if (ctx->mouse_pressed == KUI_MOUSE_LEFT && ctx->focus == id) {
    res |= KUI_RES_CHANGE;
    *state = !*state;
  }
  /* draw */
  kui_draw_control_frame(ctx, id, box, KUI_COLOR_BASE, 0);
  if (*state) {
    kui_draw_icon(ctx, KUI_ICON_CHECK, box, ctx->style->colors[KUI_COLOR_TEXT]);
  }
  r = kui_rect(r.x + box.w, r.y, r.w - box.w, r.h);
  kui_draw_control_text(ctx, label, r, KUI_COLOR_TEXT, 0);
  return res;
}


int kui_textbox_raw(kui_Context *ctx, char *buf, int bufsz, kui_Id id, kui_Rect r,
  int opt)
{
  int res = 0;
  kui_update_control(ctx, id, r, opt | KUI_OPT_HOLDFOCUS);

  if (ctx->focus == id) {
    /* handle text input */
    int len = strlen(buf);
    int n = kui_min(bufsz - len - 1, (int) strlen(ctx->input_text));
    if (n > 0) {
      memcpy(buf + len, ctx->input_text, n);
      len += n;
      buf[len] = '\0';
      res |= KUI_RES_CHANGE;
    }
    /* handle backspace */
    if (ctx->key_pressed & KUI_KEY_BACKSPACE && len > 0) {
      /* skip utf-8 continuation bytes */
      while ((buf[--len] & 0xc0) == 0x80 && len > 0);
      buf[len] = '\0';
      res |= KUI_RES_CHANGE;
    }
    /* handle return */
    if (ctx->key_pressed & KUI_KEY_RETURN) {
      kui_set_focus(ctx, 0);
      res |= KUI_RES_SUBMIT;
    }
  }

  /* draw */
  kui_draw_control_frame(ctx, id, r, KUI_COLOR_BASE, opt);
  if (ctx->focus == id) {
    kui_Color color = ctx->style->colors[KUI_COLOR_TEXT];
    kui_Font font = ctx->style->font;
    int textw = kui_r_get_text_width(kui_get_current_container(ctx), font, buf, -1);
    int texth = kui_r_get_text_height(kui_get_current_container(ctx), font);
    int ofx = r.w - ctx->style->padding - textw - 1;
    int textx = r.x + kui_min(ofx, ctx->style->padding);
    int texty = r.y + (r.h - texth) / 2;
    kui_push_clip_rect(ctx, r);
    kui_draw_text(ctx, font, buf, -1, kui_vec2(textx, texty), color);
    kui_draw_rect(ctx, kui_rect(textx + textw, texty, 1, texth), color);
    kui_pop_clip_rect(ctx);
  } else {
    kui_draw_control_text(ctx, buf, r, KUI_COLOR_TEXT, opt);
  }

  return res;
}


static int number_textbox(kui_Context *ctx, kui_Real *value, kui_Rect r, kui_Id id) {
  if (ctx->mouse_pressed == KUI_MOUSE_LEFT && ctx->key_down & KUI_KEY_SHIFT &&
      ctx->hover == id
  ) {
    ctx->number_edit = id;
    sprintf(ctx->number_edit_buf, KUI_REAL_FMT, *value);
  }
  if (ctx->number_edit == id) {
    int res = kui_textbox_raw(
      ctx, ctx->number_edit_buf, sizeof(ctx->number_edit_buf), id, r, 0);
    if (res & KUI_RES_SUBMIT || ctx->focus != id) {
      *value = strtod(ctx->number_edit_buf, NULL);
      ctx->number_edit = 0;
    } else {
      return 1;
    }
  }
  return 0;
}


int kui_textbox_ex(kui_Context *ctx, char *buf, int bufsz, int opt) {
  kui_Id id = kui_get_id(ctx, &buf, sizeof(buf));
  kui_Rect r = kui_layout_next(ctx);
  return kui_textbox_raw(ctx, buf, bufsz, id, r, opt);
}


int kui_slider_ex(kui_Context *ctx, kui_Real *value, kui_Real low, kui_Real high,
  kui_Real step, const char *fmt, int opt)
{
  char buf[KUI_MAX_FMT + 1];
  kui_Rect thumb;
  int x, w, res = 0;
  kui_Real last = *value, v = last;
  kui_Id id = kui_get_id(ctx, &value, sizeof(value));
  kui_Rect base = kui_layout_next(ctx);

  /* handle text input mode */
  if (number_textbox(ctx, &v, base, id)) { return res; }

  /* handle normal mode */
  kui_update_control(ctx, id, base, opt);

  /* handle input */
  if (ctx->focus == id &&
      (ctx->mouse_down | ctx->mouse_pressed) == KUI_MOUSE_LEFT)
  {
    v = low + (ctx->mouse_pos.x - base.x) * (high - low) / base.w;
    if (step) { v = (((v + step / 2) / step)) * step; }
  }
  /* clamp and store value, update res */
  *value = v = kui_clamp(v, low, high);
  if (last != v) { res |= KUI_RES_CHANGE; }

  /* draw base */
  kui_draw_control_frame(ctx, id, base, KUI_COLOR_BASE, opt);
  /* draw thumb */
  w = ctx->style->thumb_size;
  x = (v - low) * (base.w - w) / (high - low);
  thumb = kui_rect(base.x + x, base.y, w, base.h);
  kui_draw_control_frame(ctx, id, thumb, KUI_COLOR_BUTTON, opt);
  /* draw text  */
  sprintf(buf, fmt, v);
  kui_draw_control_text(ctx, buf, base, KUI_COLOR_TEXT, opt);

  return res;
}


int kui_number_ex(kui_Context *ctx, kui_Real *value, kui_Real step,
  const char *fmt, int opt)
{
  char buf[KUI_MAX_FMT + 1];
  int res = 0;
  kui_Id id = kui_get_id(ctx, &value, sizeof(value));
  kui_Rect base = kui_layout_next(ctx);
  kui_Real last = *value;

  /* handle text input mode */
  if (number_textbox(ctx, value, base, id)) { return res; }

  /* handle normal mode */
  kui_update_control(ctx, id, base, opt);

  /* handle input */
  if (ctx->focus == id && ctx->mouse_down == KUI_MOUSE_LEFT) {
    *value += ctx->mouse_delta.x * step;
  }
  /* set flag if value changed */
  if (*value != last) { res |= KUI_RES_CHANGE; }

  /* draw base */
  kui_draw_control_frame(ctx, id, base, KUI_COLOR_BASE, opt);
  /* draw text  */
  sprintf(buf, fmt, *value);
  kui_draw_control_text(ctx, buf, base, KUI_COLOR_TEXT, opt);

  return res;
}


static int header(kui_Context *ctx, const char *label, int istreenode, int opt) {
  kui_Rect r;
  int active, expanded;
  kui_Id id = kui_get_id(ctx, label, strlen(label));
  int idx = kui_pool_get(ctx, ctx->treenode_pool, KUI_TREENODEPOOL_SIZE, id);
  int width = -1;
  kui_layout_row(ctx, 1, &width, 0);

  active = (idx >= 0);
  expanded = (opt & KUI_OPT_EXPANDED) ? !active : active;
  r = kui_layout_next(ctx);
  kui_update_control(ctx, id, r, 0);

  /* handle click */
  active ^= (ctx->mouse_pressed == KUI_MOUSE_LEFT && ctx->focus == id);

  /* update pool ref */
  if (idx >= 0) {
    if (active) { kui_pool_update(ctx, ctx->treenode_pool, idx); }
           else { memset(&ctx->treenode_pool[idx], 0, sizeof(kui_PoolItem)); }
  } else if (active) {
    kui_pool_init(ctx, ctx->treenode_pool, KUI_TREENODEPOOL_SIZE, id);
  }

  /* draw */
  if (istreenode) {
    if (ctx->hover == id) { draw_frame(ctx, r, KUI_COLOR_BUTTONHOVER); }
  } else {
    kui_draw_control_frame(ctx, id, r, KUI_COLOR_BUTTON, 0);
  }
  kui_draw_icon(
    ctx, expanded ? KUI_ICON_EXPANDED : KUI_ICON_COLLAPSED,
    kui_rect(r.x, r.y, r.h, r.h), ctx->style->colors[KUI_COLOR_TEXT]);
  r.x += r.h - ctx->style->padding;
  r.w -= r.h - ctx->style->padding;
  kui_draw_control_text(ctx, label, r, KUI_COLOR_TEXT, 0);

  return expanded ? KUI_RES_ACTIVE : 0;
}


int kui_header_ex(kui_Context *ctx, const char *label, int opt) {
  return header(ctx, label, 0, opt);
}


int kui_begin_treenode_ex(kui_Context *ctx, const char *label, int opt) {
  int res = header(ctx, label, 1, opt);
  if (res & KUI_RES_ACTIVE) {
    get_layout(ctx)->indent += ctx->style->indent;
    push(ctx->id_stack, ctx->last_id);
  }
  return res;
}


void kui_end_treenode(kui_Context *ctx) {
  get_layout(ctx)->indent -= ctx->style->indent;
  kui_pop_id(ctx);
}


#define scrollbar(ctx, cnt, b, cs, x, y, w, h)                              \
  do {                                                                      \
    /* only add scrollbar if content size is larger than body */            \
    int maxscroll = cs.y - b->h;                                            \
                                                                            \
    if (maxscroll > 0 && b->h > 0) {                                        \
      kui_Rect base, thumb;                                                  \
      kui_Id id = kui_get_id(ctx, "!scrollbar" #y, 11);                       \
                                                                            \
      /* get sizing / positioning */                                        \
      base = *b;                                                            \
      base.x = b->x + b->w;                                                 \
      base.w = ctx->style->scrollbar_size;                                  \
                                                                            \
      /* handle input */                                                    \
      kui_update_control(ctx, id, base, 0);                                  \
      if (ctx->focus == id && ctx->mouse_down == KUI_MOUSE_LEFT) {           \
        cnt->scroll.y += ctx->mouse_delta.y * cs.y / base.h;                \
      }                                                                     \
      /* clamp scroll to limits */                                          \
      cnt->scroll.y = kui_clamp(cnt->scroll.y, 0, maxscroll);                \
                                                                            \
      /* draw base and thumb */                                             \
      draw_frame(ctx, base, KUI_COLOR_SCROLLBASE);                      \
      thumb = base;                                                         \
      thumb.h = kui_max(ctx->style->thumb_size, base.h * b->h / cs.y);       \
      thumb.y += cnt->scroll.y * (base.h - thumb.h) / maxscroll;            \
      draw_frame(ctx, thumb, KUI_COLOR_SCROLLTHUMB);                    \
                                                                            \
      /* set this as the scroll_target (will get scrolled on mousewheel) */ \
      /* if the mouse is over it */                                         \
      if (kui_mouse_over(ctx, *b)) { ctx->scroll_target = cnt; }             \
    } else {                                                                \
      cnt->scroll.y = 0;                                                    \
    }                                                                       \
  } while (0)


static void scrollbars(kui_Context *ctx, kui_Container *cnt, kui_Rect *body) {
  int sz = ctx->style->scrollbar_size;
  kui_Vec2 cs = cnt->content_size;
  cs.x += ctx->style->padding * 2;
  cs.y += ctx->style->padding * 2;
  kui_push_clip_rect(ctx, *body);
  /* resize body to make room for scrollbars */
  if (cs.y > cnt->body.h) { body->w -= sz; }
  if (cs.x > cnt->body.w) { body->h -= sz; }
  /* to create a horizontal or vertical scrollbar almost-identical code is
  ** used; only the references to `x|y` `w|h` need to be switched */
  scrollbar(ctx, cnt, body, cs, x, y, w, h);
  scrollbar(ctx, cnt, body, cs, y, x, h, w);
  kui_pop_clip_rect(ctx);
}


static void push_container_body(
  kui_Context *ctx, kui_Container *cnt, kui_Rect body, int opt
) {
  if (~opt & KUI_OPT_NOSCROLL) { scrollbars(ctx, cnt, &body); }
  push_layout(ctx, expand_rect(body, -ctx->style->padding), cnt->scroll);
  cnt->body = body;
}


static void begin_root_container(kui_Context *ctx, kui_Container *cnt) {
  push(ctx->container_stack, cnt);
  /* push container to roots list and push head command */
  push(ctx->root_list, cnt);
  cnt->head = push_jump(ctx, NULL);
  /* set as hover root if the mouse is overlapping this container and it has a
  ** higher zindex than the current hover root */
  if (rect_overlaps_vec2(cnt->rect, ctx->mouse_pos) &&
      (!ctx->next_hover_root || cnt->zindex > ctx->next_hover_root->zindex)
  ) {
    ctx->next_hover_root = cnt;
  }
  /* clipping is reset here in case a root-container is made within
  ** another root-containers's begin/end block; this prevents the inner
  ** root-container being clipped to the outer */
  push(ctx->clip_stack, unclipped_rect);
}


static void end_root_container(kui_Context *ctx) {
  /* push tail 'goto' jump command and set head 'skip' command. the final steps
  ** on initing these are done in kui_end() */
  kui_Container *cnt = kui_get_current_container(ctx);
  cnt->tail = push_jump(ctx, NULL);
  cnt->head->jump.dst = ctx->command_list.items + ctx->command_list.idx;
  /* pop base clip rect and container */
  kui_pop_clip_rect(ctx);
  pop_container(ctx);
}


int kui_begin_window_ex(kui_Context *ctx, const char *title, kui_Rect rect, int opt) {
  kui_Rect body;
  kui_Id id = kui_get_id(ctx, title, strlen(title));
  kui_Container *cnt = get_container(ctx, id, opt);
  if (!cnt || !cnt->open) { return 0; }
  if(!cnt->is_windows) kui_r_create_window(ctx, cnt, rect);
  /* we already set rect.x, rect.y as the position of the window so we don't need them anymore, because we want the component to be display at (0, 0)*/
  rect.x = 0;
  rect.y = 0;
  
  push(ctx->id_stack, id);

  if (cnt->rect.w == 0) { cnt->rect = rect; }
  begin_root_container(ctx, cnt);
  rect = body = cnt->rect;

  /* draw frame */
  if (~opt & KUI_OPT_NOFRAME) {
    draw_frame(ctx, rect, KUI_COLOR_WINDOWBG);
  }

  /* do title bar */
  if (~opt & KUI_OPT_NOTITLE) {
    kui_Rect tr = rect;
    tr.h = ctx->style->title_height;
    draw_frame(ctx, tr, KUI_COLOR_TITLEBG);

    /* do title text */
    if (~opt & KUI_OPT_NOTITLE) {
      kui_Id id = kui_get_id(ctx, "!title", 6);
      kui_update_control(ctx, id, tr, opt);
      kui_draw_control_text(ctx, title, tr, KUI_COLOR_TITLETEXT, opt);
      if (id == ctx->focus && ctx->mouse_down == KUI_MOUSE_LEFT) {
        // TODO
        // kui_r_move_window(cnt, ctx->mouse_delta.x, ctx->mouse_delta.y);
      }
      body.y += tr.h;
      body.h -= tr.h;
    }

    /* do `close` button */
    if (~opt & KUI_OPT_NOCLOSE) {
      kui_Id id = kui_get_id(ctx, "!close", 6);
      kui_Rect r = kui_rect(tr.x + tr.w - tr.h, tr.y, tr.h, tr.h);
      tr.w -= r.w;
      kui_draw_icon(ctx, KUI_ICON_CLOSE, r, ctx->style->colors[KUI_COLOR_TITLETEXT]);
      kui_update_control(ctx, id, r, opt);
      if (ctx->mouse_pressed == KUI_MOUSE_LEFT && id == ctx->focus) {
        cnt->open = 0;
      }
    }
  }

  push_container_body(ctx, cnt, body, opt);

  /* do `resize` handle */
  if (~opt & KUI_OPT_NORESIZE) {
    int sz = ctx->style->title_height;
    kui_Id id = kui_get_id(ctx, "!resize", 7);
    kui_Rect r = kui_rect(rect.x + rect.w - sz, rect.y + rect.h - sz, sz, sz);
    kui_update_control(ctx, id, r, opt);
    if (id == ctx->focus && ctx->mouse_down == KUI_MOUSE_LEFT) {
      cnt->rect.w = kui_max(96, cnt->rect.w + ctx->mouse_delta.x);
      cnt->rect.h = kui_max(64, cnt->rect.h + ctx->mouse_delta.y);
      kui_r_resize_window(cnt, cnt->rect);
    }
  }

  /* resize to content size */
  if (opt & KUI_OPT_AUTOSIZE) {
    kui_Rect r = get_layout(ctx)->body;
    cnt->rect.w = cnt->content_size.x + (cnt->rect.w - r.w);
    cnt->rect.h = cnt->content_size.y + (cnt->rect.h - r.h);
  }

  /* close if this is a popup window and elsewhere was clicked */
  if (opt & KUI_OPT_POPUP && ctx->mouse_pressed && ctx->hover_root != cnt) {
    cnt->open = 0;
  }

  kui_push_clip_rect(ctx, cnt->body);
  return KUI_RES_ACTIVE;
}


void kui_end_window(kui_Context *ctx) {
  kui_pop_clip_rect(ctx);
  end_root_container(ctx);
}


void kui_open_popup(kui_Context *ctx, const char *name) {
  kui_Container *cnt = kui_get_container(ctx, name);
  /* set as hover root so popup isn't closed in begin_window_ex()  */
  ctx->hover_root = ctx->next_hover_root = cnt;
  /* position at mouse cursor, open and bring-to-front */
  cnt->rect = kui_rect(ctx->mouse_pos.x, ctx->mouse_pos.y, 1, 1);
  cnt->open = 1;
  kui_bring_to_front(ctx, cnt);
}


int kui_begin_popup(kui_Context *ctx, const char *name) {
  int opt = KUI_OPT_POPUP | KUI_OPT_AUTOSIZE | KUI_OPT_NORESIZE |
            KUI_OPT_NOSCROLL | KUI_OPT_NOTITLE | KUI_OPT_CLOSED;
  return kui_begin_window_ex(ctx, name, kui_rect(0, 0, 0, 0), opt);
}


void kui_end_popup(kui_Context *ctx) {
  kui_end_window(ctx);
}


void kui_begin_panel_ex(kui_Context *ctx, const char *name, int opt) {
  kui_Container *cnt;
  kui_push_id(ctx, name, strlen(name));
  cnt = get_container(ctx, ctx->last_id, opt);
  cnt->rect = kui_layout_next(ctx);
  if (~opt & KUI_OPT_NOFRAME) {
    draw_frame(ctx, cnt->rect, KUI_COLOR_PANELBG);
  }
  push(ctx->container_stack, cnt);
  push_container_body(ctx, cnt, cnt->rect, opt);
  kui_push_clip_rect(ctx, cnt->body);
}


void kui_end_panel(kui_Context *ctx) {
  kui_pop_clip_rect(ctx);
  pop_container(ctx);
}
#include <stdio.h>

#include <kot-ui/renderer.h>

#include <kot-graphics/orb.h>
#include <kot-graphics/font.h>

void kui_r_event_handler(enum kot_Window_Event EventType, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3, uint64_t GP4){
  kui_Container* cnt = (kui_Container*)kot_Sys_GetExternalDataThread();
  if(EventType == Window_Event_Mouse){
        int64_t RelativePositionX = GP0 - cnt->window_parent->window->Position.x;
        int64_t RelativePositionY = GP1 - cnt->window_parent->window->Position.y;
        if(GP2){
            kui_input_scroll(cnt->window_parent->ctx, 0, GP2);
        }
        if(GP3 != cnt->window_parent->ctx->last_mouse_status){
            uint64_t PressedButton = GP3 & (~cnt->window_parent->ctx->last_mouse_status);
            if(PressedButton){
                int Button = ((PressedButton & MOUSE_CLICK_LEFT) ? KUI_MOUSE_LEFT : 0) | ((PressedButton & MOUSE_CLICK_RIGHT) ? KUI_MOUSE_RIGHT : 0) | ((PressedButton & MOUSE_CLICK_MIDDLE) ? KUI_MOUSE_MIDDLE : 0);
                kui_input_mousedown(cnt->window_parent->ctx, RelativePositionX, RelativePositionY, Button);
            }
            uint64_t UnpressedButton = (~GP3) & cnt->window_parent->ctx->last_mouse_status;
            if(UnpressedButton){
                int Button = ((UnpressedButton & MOUSE_CLICK_LEFT) ? KUI_MOUSE_LEFT : 0) | ((UnpressedButton & MOUSE_CLICK_RIGHT) ? KUI_MOUSE_RIGHT : 0) | ((UnpressedButton & MOUSE_CLICK_MIDDLE) ? KUI_MOUSE_MIDDLE : 0);
                kui_input_mouseup(cnt->window_parent->ctx, RelativePositionX, RelativePositionY, Button);
            }
            cnt->window_parent->ctx->last_mouse_status = GP3;
        }else{
            kui_input_mousemove(cnt->window_parent->ctx, RelativePositionX, RelativePositionY);
        }
  }
  cnt->window_parent->ctx->callback_frame(cnt->window_parent->ctx);
  kot_Sys_Event_Close();
}

void kui_r_create_window(kui_Context *ctx, kui_Container *cnt, kui_Rect rect){
  if(cnt->is_windows) return;
  cnt->is_windows = true;
  cnt->window_parent = (kui_Window*)malloc(sizeof(kui_Window));
  kot_Sys_Event_Create(&cnt->window_parent->window_event);
  kot_Sys_CreateThread(kot_Sys_GetProcess(), (void*)&kui_r_event_handler, PriviledgeApp, cnt, &cnt->window_parent->window_handler_thread);
  kot_Sys_Event_Bind(cnt->window_parent->window_event, cnt->window_parent->window_handler_thread, false);
  cnt->window_parent->window = CreateWindow(cnt->window_parent->window_event, Window_Type_Default);
  WindowChangePosition(cnt->window_parent->window, rect.x, rect.y);
  ResizeWindow(cnt->window_parent->window, rect.w, rect.h);
  ChangeVisibilityWindow(cnt->window_parent->window, true);

  memcpy(&cnt->window_parent->backbuffer, &cnt->window_parent->window->Framebuffer, sizeof(kot_framebuffer_t));
  cnt->window_parent->backbuffer.Buffer = calloc(1, cnt->window_parent->backbuffer.Size);

  FILE* FontFile = fopen("d0:arial.ttf", "r");
  fseek(FontFile, 0, SEEK_END);
  size64_t Size = ftell(FontFile);
  void* Buffer = malloc(Size);
  fseek(FontFile, 0, SEEK_SET);
  fread(Buffer, Size, 1, FontFile);
  cnt->window_parent->default_font = (kfont_t*)LoadFont((void*)Buffer, Size);
  free(Buffer);
  fclose(FontFile);
  LoadPen(cnt->window_parent->default_font, &cnt->window_parent->backbuffer, 0, 0, 11, 0, 0xffffff);
  cnt->window_parent->ctx = ctx;
}

void kui_r_draw_rect(kui_Container *cnt, kui_Rect rect, kui_Color color){
  if(!cnt->window_parent) return;
  cnt->window_parent->last_color = color;
  FillRect(&cnt->window_parent->backbuffer, rect.x, rect.y, rect.w, rect.h, kui_convert_color_to_kot_color(color));
}


void kui_r_draw_text(kui_Container *cnt, kui_Font font, const char *text, int len, kui_Vec2 pos, kui_Color color){
  if(!cnt->window_parent) return;
  cnt->window_parent->last_color = color;
  if(font == NULL) font = cnt->window_parent->default_font;
  if(len == -1){ 
    len = strlen(text); 
  }
  SetPenColor(font, kui_convert_color_to_kot_color(color));
  SetPenPosX(font, pos.x);
  SetPenPosY(font, pos.y);
  DrawFontN(font, text, (size_t)len);
}


void kui_r_draw_icon(kui_Container *cnt, int id, kui_Rect rect, kui_Color color){
  if(!cnt->window_parent) return;
  cnt->window_parent->last_color = color;
  // TODO
}


int kui_r_get_text_width(kui_Container *cnt, kui_Font font, const char *text, int len){
  if(!cnt->window_parent) return 0;
  if(font == NULL) font = cnt->window_parent->default_font;
  if(len == -1){ 
    len = strlen(text); 
  }
  uint64_t Width;
  uint64_t Height;
  kfont_pos_t PosX;
  kfont_pos_t PosY;
  GetTextboxInfoN(font, text, len, &Width, &Height, &PosX, &PosY);
  return (int)Width;
}


int kui_r_get_text_height(kui_Container *cnt, kui_Font font){
  if(!cnt->window_parent) return 0;
  if(font == NULL) font = cnt->window_parent->default_font;
  return (int)GetLineHeight(font);
}


void kui_r_set_clip_rect(kui_Container *cnt, kui_Rect rect){
  if(!cnt->window_parent) return;
  rect.h = cnt->window_parent->backbuffer.Height - (rect.y + rect.h);
  kui_r_draw_rect(cnt, rect, cnt->window_parent->last_color);
}


void kui_r_clear(kui_Container *cnt, kui_Color clr){
  if(!cnt->window_parent) return;
  kui_Rect Rect = {.x = 0, .y = 0, .w = cnt->window_parent->backbuffer.Width, .h = cnt->window_parent->backbuffer.Height};
  kui_r_draw_rect(cnt, Rect, clr);
}

void kui_r_present(kui_Container *cnt){
  if(!cnt->window_parent) return;
  memcpy(cnt->window_parent->window->Framebuffer.Buffer, cnt->window_parent->backbuffer.Buffer, cnt->window_parent->backbuffer.Size);
}

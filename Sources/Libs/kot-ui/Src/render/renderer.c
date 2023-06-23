#include <stdio.h>

#include <kot-ui/renderer.h>

#include <kot-graphics/orb.h>
#include <kot-graphics/font.h>

uint64_t kui_KeyboardCache = 0;
void* kui_TableConverter;
size64_t kui_TableConverterCharCount;

void kui_r_input_keyboard(kui_Context* ctx, uint64_t key){
  bool IsPressed;
  kot_GetCharFromScanCode(key, kui_TableConverter, kui_TableConverterCharCount, NULL, &IsPressed, NULL);
  switch (key){
    case 0x1C:{
      key = KUI_KEY_RETURN;
      if(IsPressed){
        kui_input_keydown(ctx, key);
      }else{
        kui_input_keyup(ctx, key);
      }
      break;
    }
    case 0x0E:{
      key = KUI_KEY_BACKSPACE;
      if(IsPressed){
        kui_input_keydown(ctx, key);
      }else{
        kui_input_keyup(ctx, key);
      }
      break;
    }
    default:{
      char Text[2];
      Text[1] = '\0';
      kot_GetCharFromScanCode(key, kui_TableConverter, kui_TableConverterCharCount, &Text[0], &IsPressed, &kui_KeyboardCache);
      if(IsPressed){
        kui_input_text(ctx, Text);
      }
      break;
    }
  }
}

void kui_r_event_handler(enum kot_Window_Event EventType, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3, uint64_t GP4){
  kui_Container* cnt = (kui_Container*)kot_Sys_GetExternalDataThread();
  kui_Context* ctx = cnt->window_parent->ctx;
  switch (EventType){
    case Window_Event_Mouse:{
      int64_t RelativePositionX = GP0 - cnt->window_parent->window->Position.x;
      int64_t RelativePositionY = GP1 - cnt->window_parent->window->Position.y;
      if(GP2){
        kui_input_scroll(cnt->window_parent->ctx, 0, GP2);
      }
      if(GP3 != ctx->last_mouse_status){
        uint64_t PressedButton = GP3 & (~ctx->last_mouse_status);
        if(PressedButton){
          int Button = ((PressedButton & MOUSE_CLICK_LEFT) ? KUI_MOUSE_LEFT : 0) | ((PressedButton & MOUSE_CLICK_RIGHT) ? KUI_MOUSE_RIGHT : 0) | ((PressedButton & MOUSE_CLICK_MIDDLE) ? KUI_MOUSE_MIDDLE : 0);
          kui_input_mousedown(ctx, RelativePositionX, RelativePositionY, Button);
        }
        uint64_t UnpressedButton = (~GP3) & ctx->last_mouse_status;
        if(UnpressedButton){
          int Button = ((UnpressedButton & MOUSE_CLICK_LEFT) ? KUI_MOUSE_LEFT : 0) | ((UnpressedButton & MOUSE_CLICK_RIGHT) ? KUI_MOUSE_RIGHT : 0) | ((UnpressedButton & MOUSE_CLICK_MIDDLE) ? KUI_MOUSE_MIDDLE : 0);
          kui_input_mouseup(ctx, RelativePositionX, RelativePositionY, Button);
        }
        ctx->last_mouse_status = GP3;
      }else{
        kui_input_mousemove(ctx, RelativePositionX, RelativePositionY);
      }
      break;
    }
    case Window_Event_Keyboard:{
      kui_r_input_keyboard(ctx, GP0);
      break;
    }
  }
  ctx->callback_frame(ctx);
  kot_Sys_Event_Close();
}

void kui_r_init(){
  kot_GetTableConverter("d0:/azerty.bin", &kui_TableConverter, &kui_TableConverterCharCount);
}

void kui_r_create_window(kui_Context *ctx, kui_Container *cnt, kui_Rect rect){
  if(cnt->is_windows) return;
  if(ctx->frame == 1) return; /* Force full calculation before creating the window */
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

  /* Load default font */
  FILE* DefaultFontFile = fopen("d0:/arial.ttf", "r");
  fseek(DefaultFontFile, 0, SEEK_END);
  size64_t DefaultFontSize = ftell(DefaultFontFile);
  void* DefaultFontBuffer = malloc(DefaultFontSize);
  fseek(DefaultFontFile, 0, SEEK_SET);
  fread(DefaultFontBuffer, DefaultFontSize, 1, DefaultFontFile);
  cnt->window_parent->default_font = LoadFont((void*)DefaultFontBuffer, DefaultFontSize);
  free(DefaultFontBuffer);
  fclose(DefaultFontFile);
  LoadPen(cnt->window_parent->default_font, &cnt->window_parent->backbuffer, 0, 0, 12, 0, 0xffffff);

  /* Load icons font */
  FILE* IconsFontFile = fopen("d0:/icons.ttf", "r");
  fseek(IconsFontFile, 0, SEEK_END);
  size64_t IconsFontSize = ftell(IconsFontFile);
  void* IconsFontBuffer = malloc(IconsFontSize);
  fseek(IconsFontFile, 0, SEEK_SET);
  fread(IconsFontBuffer, IconsFontSize, 1, IconsFontFile);
  cnt->window_parent->icons_font = LoadFont((void*)IconsFontBuffer, IconsFontSize);
  free(IconsFontBuffer);
  fclose(IconsFontFile);
  LoadPen(cnt->window_parent->icons_font, &cnt->window_parent->backbuffer, 0, 0, 12, 0, 0xffffff);

  cnt->window_parent->ctx = ctx;
}

void kui_r_move_window(kui_Container *cnt, kui_Vec2 pos){
  pos.x += (int)cnt->window_parent->window->Position.x;
  pos.y += (int)cnt->window_parent->window->Position.y;
  WindowChangePosition(cnt->window_parent->window, pos.x, pos.y);
}

void kui_r_resize_window(kui_Container *cnt, kui_Rect rect){
  ResizeWindow(cnt->window_parent->window, rect.x, rect.y);
}

void kui_r_draw_rect(kui_Container *cnt, kui_Rect rect, kui_Color color){
  if(!cnt->window_parent) return;
  if(!color.a) return;
  cnt->window_parent->last_color = color;
  FillRect(&cnt->window_parent->backbuffer, rect.x, rect.y, rect.w, rect.h, kui_convert_color_to_kot_color(color));
}


void kui_r_draw_text(kui_Container *cnt, kui_Font font, const char *text, int len, kui_Vec2 pos, kui_Color color){
  if(!cnt->window_parent) return;
  if(!color.a) return;
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
  if(!color.a) return;
  cnt->window_parent->last_color = color;
  kui_Font font = cnt->window_parent->icons_font;
  SetPenColor(font, kui_convert_color_to_kot_color(color));
  SetPenPosX(font, rect.x + 2);
  SetPenPosY(font, rect.y + 2);
  DrawGlyph(font, id, rect.w - 4, rect.h - 4);
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
  // TODO : add tester for put pixel
}


void kui_r_clear(kui_Container *cnt, kui_Color clr){
  if(!cnt->window_parent) return;
  kui_Rect Rect = {.x = 0, .y = 0, .w = cnt->window_parent->backbuffer.Width, .h = cnt->window_parent->backbuffer.Height};
  kui_r_draw_rect(cnt, Rect, clr);
}

void kui_r_framebuffer(kui_Container *cnt, kot_framebuffer_t* fb, kui_Rect rect){
  if(!cnt->window_parent) return;
  BlitFramebuffer(&cnt->window_parent->backbuffer, fb, rect.x, rect.y);
}

void kui_r_present(kui_Container *cnt){
  if(!cnt->window_parent) return;
  memcpy(cnt->window_parent->window->Framebuffer.Buffer, cnt->window_parent->backbuffer.Buffer, cnt->window_parent->backbuffer.Size);
}

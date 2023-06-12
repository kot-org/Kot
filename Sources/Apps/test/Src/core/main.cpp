#include <stdio.h>

#include <kot/sys.h>

#include <kot-ui/core.h>
#include <kot-ui/renderer.h>

static float bg[3] = { 90, 95, 100 };

static void test_window(kui_Context *ctx) {
  /* do window */
  if (kui_begin_window(ctx, "Demo Window", kui_rect(40, 40, 500, 450))) {
    kui_Container *win = kui_get_current_container(ctx);
    win->rect.w = kui_max(win->rect.w, 240);
    win->rect.h = kui_max(win->rect.h, 300);

    /* window info */
    if (kui_header(ctx, "Window Info")) {
      kui_Container *win = kui_get_current_container(ctx);
      char buf[64];
      kui_layout_row(ctx, 2, (int[]) { 54, -1 }, 0);
      kui_label(ctx,"Position:");
      sprintf(buf, "%d, %d", win->rect.x, win->rect.y); kui_label(ctx, buf);
      kui_label(ctx, "Size:");
      sprintf(buf, "%d, %d", win->rect.w, win->rect.h); kui_label(ctx, buf);
    }

    /* labels + buttons */
    if (kui_header_ex(ctx, "Test Buttons", KUI_OPT_EXPANDED)) {
      kui_layout_row(ctx, 3, (int[]) { 86, -110, -1 }, 0);
      kui_label(ctx, "Test buttons 1:");
      if (kui_button(ctx, "Button 1")) { kot_Printlog("Pressed button 1"); }
      if (kui_button(ctx, "Button 2")) { kot_Printlog("Pressed button 2"); }
      kui_label(ctx, "Test buttons 2:");
      if (kui_button(ctx, "Button 3")) { kot_Printlog("Pressed button 3"); }
      if (kui_button(ctx, "Popup")) { kui_open_popup(ctx, "Test Popup"); }
      if (kui_begin_popup(ctx, "Test Popup")) {
        kui_button(ctx, "Hello");
        kui_button(ctx, "World");
        kui_end_popup(ctx);
      }
    }

    /* tree */
    if (kui_header_ex(ctx, "Tree and Text", KUI_OPT_EXPANDED)) {
      kui_layout_row(ctx, 2, (int[]) { 140, -1 }, 0);
      kui_layout_begin_column(ctx);
      if (kui_begin_treenode(ctx, "Test 1")) {
        if (kui_begin_treenode(ctx, "Test 1a")) {
          kui_label(ctx, "Hello");
          kui_label(ctx, "world");
          kui_end_treenode(ctx);
        }
        if (kui_begin_treenode(ctx, "Test 1b")) {
          if (kui_button(ctx, "Button 1")) { kot_Printlog("Pressed button 1"); }
          if (kui_button(ctx, "Button 2")) { kot_Printlog("Pressed button 2"); }
          kui_end_treenode(ctx);
        }
        kui_end_treenode(ctx);
      }
      if (kui_begin_treenode(ctx, "Test 2")) {
        kui_layout_row(ctx, 2, (int[]) { 54, 54 }, 0);
        if (kui_button(ctx, "Button 3")) { kot_Printlog("Pressed button 3"); }
        if (kui_button(ctx, "Button 4")) { kot_Printlog("Pressed button 4"); }
        if (kui_button(ctx, "Button 5")) { kot_Printlog("Pressed button 5"); }
        if (kui_button(ctx, "Button 6")) { kot_Printlog("Pressed button 6"); }
        kui_end_treenode(ctx);
      }
      if (kui_begin_treenode(ctx, "Test 3")) {
        static int checks[3] = { 1, 0, 1 };
        kui_checkbox(ctx, "Checkbox 1", &checks[0]);
        kui_checkbox(ctx, "Checkbox 2", &checks[1]);
        kui_checkbox(ctx, "Checkbox 3", &checks[2]);
        kui_end_treenode(ctx);
      }
      kui_layout_end_column(ctx);

      kui_layout_begin_column(ctx);
      kui_layout_row(ctx, 1, (int[]) { -1 }, 0);
      kui_text(ctx, "Lorem ipsum dolor sit amet, consectetur adipiscing "
        "elit. Maecenas lacinia, sem eu lacinia molestie, mi risus faucibus "
        "ipsum, eu varius magna felis a nulla.");
      kui_layout_end_column(ctx);
    }

    /* background color sliders */
    if (kui_header_ex(ctx, "Background Color", KUI_OPT_EXPANDED)) {
      kui_layout_row(ctx, 2, (int[]) { -78, -1 }, 74);
      /* sliders */
      kui_layout_begin_column(ctx);
      kui_layout_row(ctx, 2, (int[]) { 46, -1 }, 0);
      kui_label(ctx, "Red:");   kui_slider(ctx, &bg[0], 0, 255);
      kui_label(ctx, "Green:"); kui_slider(ctx, &bg[1], 0, 255);
      kui_label(ctx, "Blue:");  kui_slider(ctx, &bg[2], 0, 255);
      kui_layout_end_column(ctx);
      /* color preview */
      kui_Rect r = kui_layout_next(ctx);
      kui_draw_rect(ctx, r, kui_color(bg[0], bg[1], bg[2], 255));
      char buf[32];
      sprintf(buf, "#%02X%02X%02X", (int) bg[0], (int) bg[1], (int) bg[2]);
      kui_draw_control_text(ctx, buf, r, KUI_COLOR_TEXT, KUI_OPT_ALIGNCENTER);
    }

    kui_end_window(ctx);
  }
}


int main(){
    kui_Context* ctx = kui_init();
    while(true){
        kui_begin(ctx);
        test_window(ctx);
        kui_end(ctx);
    }

    return 0;
}
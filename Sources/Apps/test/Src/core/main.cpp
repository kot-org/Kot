#include <stdio.h>

#include <kot/sys.h>

#include <kot-ui/core.h>
#include <kot-ui/renderer.h>

static float bg[3] = { 90, 95, 100 };

static void test_window(kui_Context *ctx) {
    if (kui_begin_window(ctx, "Log Window", kui_rect(350, 40, 300, 200))) {
        /* output text panel */
        kui_layout_row(ctx, 1, (int[]) { -1 }, -25);
        kui_begin_panel(ctx, "Log Output");
        kui_Container *panel = kui_get_current_container(ctx);
        kui_layout_row(ctx, 1, (int[]) { -1 }, -1);
        kui_text(ctx, "ok");
        kui_end_panel(ctx);

        /* input textbox + submit button */
        static char buf[128];
        int submitted = 0;
        kui_layout_row(ctx, 2, (int[]) { -70, -1 }, 0);
        if (kui_textbox(ctx, buf, sizeof(buf)) & KUI_RES_SUBMIT) {
            kui_set_focus(ctx, ctx->last_id);
            submitted = 1;
        }
        if (kui_button(ctx, "Submit")) { submitted = 1; }
        if (submitted) {
            kot_Printlog(buf);
            buf[0] = '\0';
        }

        kui_end_window(ctx);
    }
}

void ProcessFrame(kui_Context* ctx){
    kui_begin(ctx);
    test_window(ctx);
    kui_end(ctx);
}


int main(){
    kui_Context* ctx = kui_init(ProcessFrame, NULL);

    return 0;
}
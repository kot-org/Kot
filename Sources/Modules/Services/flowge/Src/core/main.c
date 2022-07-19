#include <core/main.h>
#include <layer/context.h>

int main(int argc, char* argv[], framebuffer_t* Framebuffer) {   

    Printlog("[FLOWGE] Initialization ...");
    
    kprocess_t self;

    screen_t* screen = (screen_t*) malloc(sizeof(screen_t));
    screen->fb_size = Framebuffer->framebuffer_pitch * Framebuffer->framebuffer_height;
    
    uint64_t virtualAddress = KotSpecificData.FreeMemorySpace - screen->fb_size;

    SYS_GetProcessKey(&self);
    SYS_Map(self, &virtualAddress, true, &Framebuffer->framebuffer_addr, &screen->fb_size, false);

    screen->fb_addr = virtualAddress;
    screen->width = Framebuffer->framebuffer_width;
    screen->height = Framebuffer->framebuffer_height;
    screen->pitch = Framebuffer->framebuffer_pitch;
    screen->bpp = Framebuffer->framebuffer_bpp;
    screen->btpp = screen->bpp/8;
    screen->bps = screen->btpp * screen->width;

    clear(screen);

    // gray rectangle with white border
    fillRect(screen, 100, 100, 200, 200, 0x1A1A1A);
    drawRect(screen, 100, 100, 200, 200, 0xFFFFFF);

    drawLine(screen, 10, 10, 100, 100, 0xFFFFFF);
    drawLine(screen, 200, 11, 170, 85, 0xFF00FF);
    drawLine(screen, 170, 85, 180,300, 0xFF00FF);

    layerContext_t* context = createContext(screen);
    layer_t* layer = createLayer(10, 10, 100, 100);

    if (context->layers == NULL) {
        Printlog("yes");
    } else {
        Printlog("no");
    }

    addLayer(context, layer);

    layer_t* l = (layer_t*) *context->layers;

    if (l->x==10) {
        Printlog("yes");
    } else {
        Printlog("no");
    }

    return KSUCCESS;

}
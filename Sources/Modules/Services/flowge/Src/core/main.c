#include <core/main.h>
#include <layer/context.h>

screen_t* screen;

void initContext() {
    Printlog("[FLOWGE] Start layers render ...");
    layerContext_t* context = createContext(screen);
    layer_t* layer = createLayer(10, 10, 100, 100);
    addLayer(context, layer);
    showLayer(layer);
    renderContext(context);
}

int main(int argc, char* argv[], framebuffer_t* Framebuffer) {   

    Printlog("[FLOWGE] Initialization ...");
    
    kprocess_t self;

    screen = (screen_t*) malloc(sizeof(screen_t));
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

    initContext();

    return KSUCCESS;

}
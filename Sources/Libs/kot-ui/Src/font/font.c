#include <kot/heap.h>
#include <kot/types.h>
#include <kot/memory.h>

#define _STRING_H_
#define _UINT64_T
#define SSFN_memcmp  memcmp
#define SSFN_memset  memset
#define SSFN_realloc realloc
#define SSFN_free    free
#define SSFN_CONSOLEBITMAP_TRUECOLOR
#define SSFN_IMPLEMENTATION 

#include <kot-ui/font.h>
#include <kot-ui/font/ssfn.h>

kfont_t* LoadFont(uintptr_t data){
    kfont_t* font = malloc(sizeof(kfont_t));
    
    return font;
}

void FreeFont(kfont_t* font){
    
}

void PrintFont(kfont_t* font, char* str, font_fb_t* buffer, uint64_t x, uint64_t y, uint32_t color){
    
}
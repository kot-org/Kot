#ifndef KOT_GRAPHICS_IMAGE
#define KOT_GRAPHICS_IMAGE

#include <kot-graphics/utils.h>

typedef struct{
	uint64_t width;
    uint64_t height;
    size_t buffer_size;
	uint32_t* buffer;
}raw_image_t;

raw_image_t* load_tga_image_file(char* file_path);
raw_image_t* load_jpeg_image_file(char* file_path);
raw_image_t* resize_image(raw_image_t* raw_image, uint16_t new_width, uint16_t new_height, bool keep_ratio);
int draw_image(kframebuffer_t* fb, raw_image_t* raw_image, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
int draw_image_with_binary_transparency(kframebuffer_t* fb, raw_image_t* raw_image, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
int free_raw_image(raw_image_t* raw_image);

#endif // KOT_GRAPHICS_IMAGE
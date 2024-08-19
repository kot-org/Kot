#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>

#include <jerror.h>
#include <jpeglib.h>

#include <kot-graphics/image.h>

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

typedef struct {
    uint8_t id_length;
    uint8_t color_map_type;
    uint8_t image_type;
    uint16_t color_map_origin, color_map_length;
    uint8_t color_map_ent_sz;
    uint16_t x, y;
    uint16_t width, height;
    uint8_t bpp;
    uint8_t image_descriptor;
} __attribute__((__packed__)) tga_header_t;

raw_image_t* load_tga_image_file(char* file_path){
    FILE* file_handle = fopen(file_path, "rb");

    if (file_handle == NULL) {
        return NULL;
    }

    tga_header_t header;
    fread(&header, sizeof(tga_header_t), 1, file_handle);

    if (header.image_type != 2 || header.bpp != 32) {
        fclose(file_handle);
        return NULL;
    }

    raw_image_t* raw_image = (raw_image_t*)malloc(sizeof(raw_image_t));
    if (raw_image == NULL) {
        fclose(file_handle);
        return NULL;
    }

    raw_image->width = header.width;
    raw_image->height = header.height;
    raw_image->buffer_size = raw_image->width * raw_image->height * (header.bpp / 8);
    raw_image->buffer = (uint32_t*)malloc(raw_image->buffer_size);
    if (raw_image->buffer == NULL) {
        free(raw_image);
        fclose(file_handle);
        return NULL;
    }

    fseek(file_handle, sizeof(tga_header_t) + header.id_length, SEEK_SET);
    fread(raw_image->buffer, raw_image->buffer_size, 1, file_handle);

    fclose(file_handle);

    return raw_image;
}

raw_image_t* load_jpeg_image_file(char* file_path){
	FILE* file_handle = fopen(file_path, "rb");

	if(file_handle == NULL) {
		return NULL; 
	}

	struct jpeg_decompress_struct info;
	struct jpeg_error_mgr error;

	info.err = jpeg_std_error(&error);
	jpeg_create_decompress(&info);

	jpeg_stdio_src(&info, file_handle);
	jpeg_read_header(&info, true);

	jpeg_start_decompress(&info);

	size_t buffer_size = info.output_width * info.output_height * sizeof(uint32_t);
	char* buffer = (char*)malloc(buffer_size);

	raw_image_t* raw_image = (raw_image_t*)malloc(sizeof(raw_image_t));
	raw_image->width = info.output_width;
	raw_image->height = info.output_height;
	raw_image->buffer = (uint32_t*)buffer;
	raw_image->buffer_size = buffer_size;

    unsigned char* row_buffer[1];
	while(info.output_scanline < info.output_height) {
        uint64_t start_index = info.output_width * info.output_scanline * sizeof(uint32_t);
        row_buffer[0] = (unsigned char*)(&buffer[start_index]);
		jpeg_read_scanlines(&info, (JSAMPARRAY)row_buffer, 1);
        for(uint64_t x = info.output_width; x > 0; x--){
            /* Convert BGR to RGBA */
            uint8_t b = buffer[(x * 3) + start_index];
            uint8_t g = buffer[(x * 3) + start_index + 1];
            uint8_t r = buffer[(x * 3) + start_index + 2];

            buffer[(x * sizeof(uint32_t)) + start_index] = r;
            buffer[(x * sizeof(uint32_t)) + start_index + 1] = g;
            buffer[(x * sizeof(uint32_t)) + start_index + 2] = b;
            buffer[(x * sizeof(uint32_t)) + start_index + 3] = 0xff;
        }
	}

	jpeg_finish_decompress(&info);
	jpeg_destroy_decompress(&info);
	fclose(file_handle);

	return raw_image;
}

int free_raw_image(raw_image_t* raw_image){
    free(raw_image->buffer);
    free(raw_image);
    return 0;
}

raw_image_t* resize_image(raw_image_t* raw_image, uint16_t new_width, uint16_t new_height, bool keep_ratio){
    if(keep_ratio){
        if(new_width == 0){
            new_width = DIV_ROUND_UP(new_height * raw_image->width, raw_image->height);
        }else if(new_height == 0){
            new_height = DIV_ROUND_UP(new_width * raw_image->height, raw_image->width);
        }
    }

    size_t buffer_size = new_width * new_height * sizeof(uint32_t);
	char* buffer = (char*)malloc(buffer_size);

	raw_image_t* new_raw_image = (raw_image_t*)malloc(sizeof(raw_image_t));
	new_raw_image->width = new_width;
	new_raw_image->height = new_height;
	new_raw_image->buffer = (uint32_t*)buffer;
	new_raw_image->buffer_size = buffer_size;

    for(uint16_t x = 0; x < new_width; x++) {
        uint32_t new_x = x * raw_image->width / new_width;
        for(uint16_t y = 0; y < new_height; y++) {
            uint32_t new_y = y * raw_image->height / new_height;
            new_raw_image->buffer[x + y * new_width] = raw_image->buffer[new_x + new_y * raw_image->width];
        }
    }

    return new_raw_image;
}

int draw_image(kframebuffer_t* fb, raw_image_t* raw_image, uint32_t x, uint32_t y, uint32_t width, uint32_t height){
    uint32_t* fb_buffer = (uint32_t*)fb->buffer;
    for(uint64_t i = 0; i < raw_image->width && i < fb->width && i < width; i++){
        for(uint64_t j = 0; j < raw_image->height && j < fb->height && j < height; j++){
            fb_buffer[(i + x) + (j + y) * fb->width] = raw_image->buffer[i + j * raw_image->width];
        }
    }

    return 0;    
}

int draw_image_with_binary_transparency(kframebuffer_t* fb, raw_image_t* raw_image, uint32_t x, uint32_t y, uint32_t width, uint32_t height){
    uint32_t* fb_buffer = (uint32_t*)fb->buffer;
    for(uint64_t i = 0; i < raw_image->width && i < fb->width && i < width; i++){
        for(uint64_t j = 0; j < raw_image->height && j < fb->height && j < height; j++){
            uint32_t color = raw_image->buffer[i + j * raw_image->width];
            if((color & 0xff000000) > 127){
                fb_buffer[(i + x) + (j + y) * fb->width] = color;
            }
        }
    }

    return 0;    
}
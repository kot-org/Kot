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

	size_t buffer_size = info.output_width * info.output_height * 3;
	char* buffer = (char*)malloc(buffer_size);

	raw_image_t* raw_image = (raw_image_t*)malloc(sizeof(raw_image_t));
	raw_image->width = info.output_width;
	raw_image->height = info.output_height;
	raw_image->buffer = buffer;
	raw_image->buffer_size = buffer_size;

    unsigned char* row_buffer[1];
	while(info.output_scanline < info.output_height) {
        row_buffer[0] = (unsigned char*)(&buffer[info.output_width * info.output_scanline * 3]);
		jpeg_read_scanlines(&info, (JSAMPARRAY)row_buffer, 1);
        for(uint64_t x = 0; x < info.output_width; x++){
            /* Convert BGR to RGB */
            uint8_t b = buffer[(x + (info.output_width * (info.output_scanline - 1))) * 3];
            uint8_t r = buffer[(x + (info.output_width * (info.output_scanline - 1))) * 3 + 2];

            buffer[(x + (info.output_width * (info.output_scanline - 1))) * 3] = r;
            buffer[(x + (info.output_width * (info.output_scanline - 1))) * 3 + 2] = b;
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

    size_t buffer_size = new_width * new_height * 3;
	char* buffer = (char*)malloc(buffer_size);

	raw_image_t* new_raw_image = (raw_image_t*)malloc(sizeof(raw_image_t));
	new_raw_image->width = new_width;
	new_raw_image->height = new_height;
	new_raw_image->buffer = buffer;
	new_raw_image->buffer_size = buffer_size;

    for(uint16_t x = 0; x < new_width; x++) {
        uint32_t new_x = x * raw_image->width / new_width;
        for(uint16_t y = 0; y < new_height; y++) {
            uint32_t new_y = y * raw_image->height / new_height;
            new_raw_image->buffer[(x + y * new_width) * 3] = raw_image->buffer[(new_x + new_y * raw_image->width) * 3];
            new_raw_image->buffer[(x + y * new_width) * 3 + 1] = raw_image->buffer[(new_x + new_y * raw_image->width) * 3 + 1];
            new_raw_image->buffer[(x + y * new_width) * 3 + 2] = raw_image->buffer[(new_x + new_y * raw_image->width) * 3 + 2];
        }
    }

    return new_raw_image;
}

int draw_image(kframebuffer_t* fb, raw_image_t* raw_image, uint32_t x, uint32_t y, uint32_t width, uint32_t height){
    uint32_t* fb_buffer = (uint32_t*)fb->buffer;
    for(uint64_t i = 0; i < raw_image->width && i < fb->width && i < width; i++){
        for(uint64_t j = 0; j < raw_image->height && j < fb->height && j < height; j++){
            fb_buffer[(i + x) + (j + y) * fb->width] = *(uint32_t*)&raw_image->buffer[(i + j * raw_image->width) * 3];
        }
    }

    return 0;    
}
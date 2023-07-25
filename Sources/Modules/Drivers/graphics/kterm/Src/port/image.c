#include "image.h"
#include "term.h"

#define DIV_ROUNDUP(A, B) \
({ \
    typeof(A) _a_ = A; \
    typeof(B) _b_ = B; \
    (_a_ + (_b_ - 1)) / _b_; \
})

#define ALIGN_UP(A, B) \
({ \
    typeof(A) _a__ = A; \
    typeof(B) _b__ = B; \
    DIV_ROUNDUP(_a__, _b__) * _b__; \
})

bool bmp_open_image(struct image_t *image, uint64_t file, uint64_t size)
{
    struct bmp_header header;
    memset(&header, 0, sizeof(struct bmp_header));
    memcpy(&header, (uint8_t*)file, sizeof(struct bmp_header));

    char signature[2] = {
        (char)(header.bf_signature & 0xFF),
        (char)(header.bf_signature >> 8)
    };
    if (signature[0] != 'B' || signature[1] != 'M') return false;

    if (header.bi_bpp % 8 != 0) return false;

    uint32_t bf_size;
    if (header.bf_offset + header.bf_size > size)
        bf_size = size - header.bf_offset;
    else bf_size = header.bf_size;

    image->img = alloc_mem(bf_size);
    memcpy(image->img, (uint8_t*)(file + header.bf_offset), bf_size);

    image->allocated_size = bf_size;

    image->x_size = header.bi_width;
    image->y_size = header.bi_height;
    image->pitch = ALIGN_UP(header.bi_width * header.bi_bpp, 32) / 8;
    image->bpp = header.bi_bpp;
    image->img_width = header.bi_width;
    image->img_height = header.bi_height;

    return true;
}

void image_make_centered(struct image_t *image, int frame_x_size, int frame_y_size, uint32_t back_colour)
{
    image->type = CENTERED;

    image->x_displacement = frame_x_size / 2 - image->x_size / 2;
    image->y_displacement = frame_y_size / 2 - image->y_size / 2;
    image->back_colour = back_colour;
}

void image_make_stretched(struct image_t *image, int new_x_size, int new_y_size)
{
    image->type = STRETCHED;

    image->x_size = new_x_size;
    image->y_size = new_y_size;
}

bool image_open(struct image_t *image, uint64_t file, uint64_t size)
{
    image->type = TILED;
    if (bmp_open_image(image, file, size))
        return true;
    return false;
}

void image_close(struct image_t *image)
{
    free_mem(image->img, image->allocated_size);
}
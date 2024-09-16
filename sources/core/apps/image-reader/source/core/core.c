#include <math.h>
#include <fcntl.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdbool.h>

#include <linux/fb.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <cjson/cJSON.h> 

#include <kot-graphics/font.h>
#include <kot-graphics/utils.h>
#include <kot-graphics/image.h>

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

#define HEADER_BACKGROUND       (0x111111)
#define IMAGE_BORDER            (0xc70039)
#define WIDTH_MARGIN            (50)
#define HEADER_SIZE             (14)
#define HEADER_HEIGHT           (26)
#define TEXT_COLOR              (0xffffff)
#define INFO_SIZE               (16)

kfont_t font;

int fb_fd = -1;
kframebuffer_t fb;
struct fb_fix_screeninfo fix_screeninfo;
struct fb_var_screeninfo var_screeninfo;

char* font_path = NULL;
char* wallpaper_path = NULL;

size_t json_size = 0;
void* json_buffer = NULL;
FILE* json_file = NULL;
cJSON* json_root = NULL;
cJSON* font_path_json = NULL;
cJSON* wallpaper_path_json = NULL;

void draw_frame(){
    write(fb_fd, fb.buffer, fb.size);
}

int get_key(int* pressed, uint64_t* key){
    uint64_t buffer[2];
    if(read(fb_fd, &buffer, sizeof(uint64_t) * 2) > 0){
        if(buffer[1] & ((uint64_t)1 << 63)){
            *pressed = true;
        }else{
            *pressed = false;
        }
        *key = buffer[0];
        return 1;
    }
    return 0;
}

char* get_file_ext(char* name) {
    char* dot = strrchr(name, '.');
    if(!dot || dot == name) return "";
    return dot + 1;
}

int wait_escape(){
    int pressed;
    uint64_t key;

    if(get_key(&pressed, &key)){
        if(pressed && key == 1){
            return 1;
        }
    }
    return 0;
}

int load_fb(){
    fb_fd = open("/dev/fb0", O_RDWR);

    assert(fb_fd >= 0);


    assert(ioctl(fb_fd, FBIOGET_FSCREENINFO, &fix_screeninfo) == 0);
    assert(ioctl(fb_fd, FBIOGET_VSCREENINFO, &var_screeninfo) == 0);

    if(
        fix_screeninfo.visual == FB_VISUAL_TRUECOLOR && 
        var_screeninfo.bits_per_pixel == 32 &&
        var_screeninfo.red.length == 8 && var_screeninfo.red.msb_right == 0 &&
        var_screeninfo.green.length == 8 && var_screeninfo.green.msb_right == 0 &&
        var_screeninfo.blue.length == 8 && var_screeninfo.blue.msb_right == 0 
    ){
        fb.bpp = var_screeninfo.bits_per_pixel;
        fb.btpp = fb.bpp / 8;
        fb.size = var_screeninfo.xres_virtual * var_screeninfo.yres_virtual * fb.btpp;
        fb.width = var_screeninfo.xres_virtual;
        fb.pitch = fb.width * fb.btpp;
        fb.height = var_screeninfo.yres_virtual;
        fb.buffer = malloc(fb.size);
        memset(fb.buffer, 0, fb.size);
    }else{
        perror("'/dev/fb0' : format not supported");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int load_json(){
    json_file = fopen("/usr/bin/res/image-reader/image-reader.json", "r"); 

    if(json_file == NULL){ 
        perror("Error: Unable to open the file : /usr/bin/res/image-reader/image-reader.json\n"); 
        return EXIT_FAILURE; 
    } 
  
    fseek(json_file, 0, SEEK_END);
    json_size = ftell(json_file);
    fseek(json_file, 0, SEEK_SET);
    
    json_buffer = malloc(json_size);
    fread(json_buffer, 1, json_size, json_file); 
  
    json_root = cJSON_Parse(json_buffer); 

    int r = 0;


    if(json_root != NULL){
        font_path_json = cJSON_GetObjectItem(json_root, "font_path");
        if(cJSON_IsString(font_path_json) && (font_path_json->valuestring != NULL)){
            font_path = strdup(font_path_json->valuestring);
        }else{
            free(json_buffer);

            fclose(json_file);
            
            printf("Error: Unable to parse the file : /usr/bin/res/image-reader/image-reader.json\n"); 
            return EXIT_FAILURE; 
        }

        wallpaper_path_json = cJSON_GetObjectItem(json_root, "wallpaper_path");
        if(cJSON_IsString(wallpaper_path_json) && (wallpaper_path_json->valuestring != NULL)){
            wallpaper_path = strdup(wallpaper_path_json->valuestring);
        }else{
            free(json_buffer);

            fclose(json_file);
            
            printf("Error: Unable to parse the file : /usr/bin/res/image-reader/image-reader.json\n"); 
            return EXIT_FAILURE; 
        }
    }else{
        free(json_buffer);

        fclose(json_file);

        printf("Error: Unable to parse the file : /usr/bin/res/image-reader/image-reader.json\n"); 
        return EXIT_FAILURE; 
    }

    return EXIT_SUCCESS;
}

int load_font_data(){
    FILE* font_file = fopen(font_path, "rb");
    if(font_file == NULL){
        perror("Failed to open font file");
        fclose(json_file);
        close(fb_fd);
        return EXIT_FAILURE;
    }

    fseek(font_file, 0, SEEK_END);
    size_t font_file_size = ftell(font_file);
    fseek(font_file, 0, SEEK_SET);

    void* font_data = malloc(font_file_size);
    fread(font_data, font_file_size, 1, font_file);
    fclose(font_file);

    font = load_font(font_data, font_file_size);
    free(font_data);

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]){
    if(argc == 2){
        if(load_fb() != EXIT_SUCCESS){
            return EXIT_FAILURE;
        }

        if(load_json() != EXIT_SUCCESS){
            return EXIT_FAILURE;
        }

        if(load_font_data(font_path) != EXIT_SUCCESS){
            cJSON_Delete(json_root);

            free(json_buffer);

            fclose(json_file);

            return EXIT_FAILURE;
        }

        char* extension = get_file_ext(argv[1]);
        raw_image_t* image = NULL;
        raw_image_t* image_resized = NULL;

        if(!strcmp(extension, "jpg") || !strcmp(extension, "jpeg")){
            image = load_jpeg_image_file(argv[1]);
        }else if(!strcmp(extension, "tga")){
            image = load_tga_image_file(argv[1]);
        }

        if(image != NULL){
            uint16_t new_height = fb.height - HEADER_HEIGHT - INFO_SIZE - 100;
            uint16_t new_width = DIV_ROUND_UP(new_height * image->width, image->height);
            if(new_width > fb.width - WIDTH_MARGIN){
                new_width = fb.width - WIDTH_MARGIN;
                new_height = DIV_ROUND_UP(new_width * image->height, image->width);
            }
            image_resized = resize_image(image, new_width, new_height, false);

            if(image_resized != NULL){
                raw_image_t* wallpaper = load_jpeg_image_file(wallpaper_path);
                raw_image_t* wallpaper_resized = NULL;
                if(wallpaper != NULL){
                    if(wallpaper->width < wallpaper->height){
                        wallpaper_resized = resize_image(wallpaper, 0, fb.height, true);
                    }else{
                        wallpaper_resized = resize_image(wallpaper, fb.width, 0, true);
                    }
                    draw_image(&fb, wallpaper_resized, 0, HEADER_HEIGHT, fb.width, fb.height - HEADER_HEIGHT);
                }

                uint32_t image_x = (fb.width - image_resized->width) / 2;
                uint32_t image_y = (fb.height - image_resized->height) / 2;
                draw_rectangle_border(&fb, image_x - 1, image_y - 1, image_resized->width + 2, image_resized->height + 2, IMAGE_BORDER);
                draw_image_with_binary_transparency(&fb, image_resized, image_x, image_y, image_resized->width, image_resized->height);
                draw_rectangle(&fb, 0, 0, fb.width, HEADER_HEIGHT, HEADER_BACKGROUND);

                load_pen(font, &fb, 0, 0, HEADER_SIZE, 0, TEXT_COLOR);
                char* image_reader_info;
                assert(asprintf(&image_reader_info, "%s | %dx%d", argv[1], image->width, image->height) >= 0);
                write_paragraph(font, 0, 0, fb.width, PARAGRAPH_CENTER, image_reader_info, -1);

                set_pen_size(font, INFO_SIZE);
                kfont_pos_t x = get_pen_pos_x(font);
                kfont_pos_t y = fb.height - INFO_SIZE - 50;
                set_pen_color(font, ~TEXT_COLOR);
                write_paragraph(font, -1, fb.height - INFO_SIZE - 50, fb.width, PARAGRAPH_CENTER, "Exit : <Esc>\n", -1);
                set_pen_color(font, TEXT_COLOR);
                write_paragraph(font, x - 1, y - 1, fb.width, PARAGRAPH_CENTER, "Exit : <Esc>\n", -1);

                draw_frame();

                while(!wait_escape());
            }else{
                printf("\033[1;31mcan't read this file: %s\n\033[0m", argv[1]); 
                printf("Press <Enter> to Continue\n");
                getchar();            
            }
        }else{
            printf("\033[1;31mcan't read this file: %s\n\033[0m", argv[1]); 
            printf("Press <Enter> to Continue\n");
            getchar();            
        }

        cJSON_Delete(json_root);

        free(json_buffer);

        fclose(json_file);

        return EXIT_SUCCESS;
    }else{
        printf("\033[1;31musage: image-reader file_path\033[0m"); 
        printf("Press <Enter> to Continue\n");
        getchar();
        return EXIT_FAILURE;
    }
}
#include <math.h>
#include <fcntl.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>

#include <stdbool.h>

#include <linux/fb.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#include <cjson/cJSON.h> 

#include <kot-graphics/font.h>
#include <kot-graphics/utils.h>
#include <kot-graphics/image.h>

#define DATE_TIME_COLOR         (0xffffff)
#define DATE_TIME_BACKGROUND    (0x111111)
#define DATE_TIME_SIZE          (14)
#define DATE_TIME_HEIGHT        (26)

kfont_t font;

int fb_fd = -1;
kframebuffer_t fb;
struct fb_fix_screeninfo fix_screeninfo;
struct fb_var_screeninfo var_screeninfo;

size_t json_size = 0;
void* json_buffer = NULL;
FILE* json_file = NULL;
cJSON* json_root = NULL;
cJSON* wallpaper_path_json = NULL;
cJSON* font_path_json = NULL;

uint64_t get_ticks_ms(){
    struct timeval tv; 
    gettimeofday(&tv, NULL);
    uint64_t milliseconds = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return milliseconds;
}

void draw_frame(){
    write(fb_fd, fb.buffer, fb.size);
}

int get_key(int* pressed, uint64_t* key){
    int64_t buffer;
    if(read(fb_fd, &buffer, 1) > 0){
        if(buffer & ((uint64_t)1 << 63)){
            *pressed = true;
        }else{
            *pressed = false;
        }
        *key = buffer & ~((uint64_t)1 << 63);
        return 1;
    }
    return 0;
}

void get_current_date_time(char *time_str) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(time_str, 20, "%B %d %H:%M", t);
}

int main(int argc, char* argv[]){
    fb_fd = open("/dev/fb0", O_RDWR);

    printf("Loading desktop...\n");

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
        perror("'/dev/fb0' : format not supported\n");
        return EXIT_FAILURE;
    }

    json_file = fopen("/usr/bin/res/desktop/desktop.json", "r+"); 

    if(json_file == NULL){ 
        fclose(json_file);

        perror("Error: Unable to open the file : /usr/bin/res/desktop/desktop.json\n"); 
        return EXIT_FAILURE; 
    } 
  
    fseek(json_file, 0, SEEK_END);
    json_size = ftell(json_file);
    fseek(json_file, 0, SEEK_SET);
    
    json_buffer = malloc(json_size);
    fread(json_buffer, 1, json_size, json_file); 
  
    json_root = cJSON_Parse(json_buffer); 

    int r = 0;

    char* wallpaper_path = NULL;
    char* font_path = NULL;

    if(json_root != NULL){ 
        wallpaper_path_json = cJSON_GetObjectItem(json_root, "wallpaper_path");
        if(cJSON_IsString(wallpaper_path_json) && (wallpaper_path_json->valuestring != NULL)){
            wallpaper_path = malloc(strlen(wallpaper_path_json->valuestring) + 1);
            strcpy(wallpaper_path, wallpaper_path_json->valuestring);
        }else{
            fclose(json_file);

            printf("Error: Unable to parse the file : /usr/bin/res/desktop/desktop.json\n"); 
            return EXIT_FAILURE; 
        }
        
        font_path_json = cJSON_GetObjectItem(json_root, "font_path");
        if(cJSON_IsString(font_path_json) && (font_path_json->valuestring != NULL)){
            font_path = malloc(strlen(font_path_json->valuestring) + 1);
            strcpy(font_path, font_path_json->valuestring);
        }else{
            fclose(json_file);
            
            printf("Error: Unable to parse the file : /usr/bin/res/desktop/desktop.json\n"); 
            return EXIT_FAILURE; 
        }
    }else{
        fclose(json_file);

        printf("Error: Unable to parse the file : /usr/bin/res/desktop/desktop.json\n"); 
        return EXIT_FAILURE; 
    }

    raw_image_t* wallpaper = load_jpeg_image_file(wallpaper_path);
    raw_image_t* wallpaper_resized;
    if(wallpaper->width < wallpaper->height){
        wallpaper_resized = resize_image(wallpaper, 0, fb.height, true);
    }else{
        wallpaper_resized = resize_image(wallpaper, fb.width, 0, true);
    }

    free_raw_image(wallpaper);

    FILE* font_file = fopen(font_path, "rb");

    if(font_file == NULL){
        free_raw_image(wallpaper_resized);

        fclose(json_file);

        perror("error loading font\n");
        return EXIT_FAILURE;
    }

    fseek(font_file, 0, SEEK_END);
    size_t font_file_size = ftell(font_file);
    fseek(font_file, 0, SEEK_SET);

    void* font_data = malloc(font_file_size);
    fread(font_data, font_file_size, 1, font_file);

    font = load_font(font_data, font_file_size);

    fclose(font_file);

    while(true){
        while(true){
            char date_time_str[20];

            draw_image(&fb, wallpaper_resized, 0, 0, fb.width, fb.height);
            draw_rectangle(&fb, 0, 0, fb.width, DATE_TIME_HEIGHT, DATE_TIME_BACKGROUND);

            get_current_date_time(date_time_str);

            load_pen(font, &fb, 0, 0, DATE_TIME_SIZE, 0, DATE_TIME_COLOR);
            write_paragraph(font, -1, -1, fb.width, PARAGRAPH_CENTER, date_time_str);

            draw_frame();
        }
    }

    free_raw_image(wallpaper_resized);

    fclose(json_file);

    return EXIT_FAILURE;
}
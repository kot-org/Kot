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

#include <curl/curl.h>

#include <cjson/cJSON.h> 

#include <kot-graphics/font.h>
#include <kot-graphics/utils.h>
#include <kot-graphics/image.h>

#include "../apps/apps.h"
#include "../launch/launch.h"
#include "../update/update.h"
#include "../install/install.h"
#include "../remove/remove.h"

int fb_fd = -1;
kframebuffer_t fb;
CURL* curl = NULL;
kfont_t font = NULL;
char* font_path = NULL;
struct fb_fix_screeninfo fix_screeninfo;
struct fb_var_screeninfo var_screeninfo;

size_t json_size = 0;
void* json_buffer = NULL;
FILE* json_file = NULL;
cJSON* json_root = NULL;
cJSON* font_path_json = NULL;

uint32_t header_part_width = 0;
char* header_text[] = {"Install <F1>", "Update <F2>", "Uninstall <F3>"};

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

void unload_fb(){
    free(fb.buffer);
    close(fb_fd);
}

int load_json(){
    json_file = fopen("/usr/bin/res/store-ui/store-ui.json", "r"); 

    if(json_file == NULL){ 
        perror("Error: Unable to open the file : /usr/bin/res/store-ui/store-ui.json\n"); 
        return EXIT_FAILURE; 
    } 
  
    fseek(json_file, 0, SEEK_END);
    json_size = ftell(json_file);
    fseek(json_file, 0, SEEK_SET);
    
    json_buffer = malloc(json_size);
    fread(json_buffer, 1, json_size, json_file); 
  
    json_root = cJSON_Parse(json_buffer); 

    if(json_root != NULL){
        font_path_json = cJSON_GetObjectItem(json_root, "font_path");
        if(cJSON_IsString(font_path_json) && (font_path_json->valuestring != NULL)){
            font_path = strdup(font_path_json->valuestring);
        }else{
            free(json_buffer);

            fclose(json_file);
            
            printf("Error: Unable to parse the file : /usr/bin/res/store-ui/store-ui.json\n"); 
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

void unload_json(){
    free(font_path);
}

int load_font_data(){
    FILE* font_file = fopen(font_path, "rb");
    if(font_file == NULL){
        perror("Failed to open font file");
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

void unload_font_data(){
    free_font(font);
}

int load_curl(){
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl == NULL){
        perror("Can't load curl");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void unload_curl(){
    curl_easy_cleanup(curl);

    curl_global_cleanup();
}

int main(int argc, char *argv[]){
    if(load_fb() != EXIT_SUCCESS){
        return EXIT_FAILURE;
    }

    if(load_json() != EXIT_SUCCESS){
        unload_fb();
        return EXIT_FAILURE;
    }

    if(load_font_data(font_path) != EXIT_SUCCESS){
        unload_json();
        unload_fb();
        return EXIT_FAILURE;
    }

    if(load_curl() != EXIT_SUCCESS){
        unload_fb();
        unload_font_data();
        return EXIT_FAILURE;
    }

    while(true){
        
    }

    unload_fb();
    unload_curl();
    unload_json();
    unload_font_data();

    return EXIT_SUCCESS;
}

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

#define FOCUS_COLOR         (0x33b2ff)
#define TEXT_COLOR          (0xffffff)
#define HEADER_COLOR        (0xeeeeee)
#define SEARCH_BAR_COLOR    (0xbbbbbb)
#define TITLE_SIZE          (22)
#define TEXT_SIZE           (16)
#define SEARCH_BAR_SIZE     (24)
#define SEARCH_MAX_CHAR     (30)
#define HEADER_TEXT_SIZE    (16)
#define HEADER_MARGIN       (20)
#define HEADER_TEXT_MARGIN  (5)
#define HEADER_HEIGHT       (HEADER_TEXT_SIZE + (HEADER_TEXT_MARGIN * 2))

int fb_fd = -1;
kframebuffer_t fb;
CURL* curl = NULL;
kfont_t font = NULL;
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

char* header_text[] = {"Install <F1>", "Update <F2>", "Uninstall <F3>", "Quit <Esc>"};
uint32_t header_part_width = 0;
int header_part_count = sizeof(header_text) / sizeof(char*);

bool update_header = true;
bool update_wallpaper = true;

int current_task = 0;

uint32_t content_x = 0;
uint32_t content_y = 0;
uint32_t content_width = 0;
uint32_t content_height = 0;

int get_key(int* pressed, uint64_t* key, uint16_t* translated_key){
    uint64_t buffer[2];
    if(read(fb_fd, &buffer, sizeof(uint64_t) * 2) > 0){
        if(buffer[1] & ((uint64_t)1 << 63)){
            *pressed = true;
        }else{
            *pressed = false;
        }
        *key = buffer[0];
        *translated_key = (buffer[1] & 0xffff);
        return 1;
    }
    return 0;
}

void draw_frame(){
    write(fb_fd, fb.buffer, fb.size);
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

        wallpaper_path_json = cJSON_GetObjectItem(json_root, "wallpaper_path");
        if(cJSON_IsString(wallpaper_path_json) && (wallpaper_path_json->valuestring != NULL)){
            wallpaper_path = strdup(wallpaper_path_json->valuestring);
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

    header_part_width = (fb.width - (HEADER_MARGIN * 2)) / header_part_count;

    load_pen(font, &fb, 0, 0, HEADER_TEXT_SIZE, 0, HEADER_COLOR);

    content_x = HEADER_MARGIN;
    content_y = HEADER_HEIGHT + (HEADER_MARGIN * 2);
    content_width = fb.width - (HEADER_MARGIN * 2);
    content_height = fb.height - HEADER_HEIGHT - content_y;

    while(true){
        if(update_wallpaper){
            update_wallpaper = false;

            raw_image_t* wallpaper = load_jpeg_image_file(wallpaper_path);
            raw_image_t* wallpaper_resized = NULL;
            if(wallpaper != NULL){
                if(wallpaper->width < wallpaper->height){
                    wallpaper_resized = resize_image(wallpaper, 0, fb.height, true);
                }else{
                    wallpaper_resized = resize_image(wallpaper, fb.width, 0, true);
                }
                draw_image(&fb, wallpaper_resized, 0, 0, fb.width, fb.height);
            }
        }
        if(update_header){
            update_header = false;
            
            for(int i = 0; i < header_part_count; i++){
                uint32_t x = i * header_part_width + HEADER_MARGIN;
                if(i == current_task){
                    set_pen_color(font, ~HEADER_COLOR);
                    draw_rectangle(&fb, x, HEADER_MARGIN, header_part_width, HEADER_HEIGHT, HEADER_COLOR);
                }else{
                    set_pen_color(font, HEADER_COLOR);
                    draw_rectangle(&fb, x + 1, HEADER_MARGIN + 1, header_part_width - 2, HEADER_HEIGHT - 2, ~HEADER_COLOR);
                    draw_rectangle_border(&fb, x, HEADER_MARGIN, header_part_width, HEADER_HEIGHT, HEADER_COLOR);
                }
                write_paragraph(font, x, HEADER_MARGIN, header_part_width, PARAGRAPH_CENTER, header_text[i], -1);
            }
        }

        switch (current_task){
            case 0:{
                static app_info_t** apps_found = NULL;
                static bool search_with_tag = true;
                static bool draw_search_box = true;
                static bool draw_content_box = true;
                static bool already_search = false;
                static char* search_bar_str = NULL;
                static int app_focus = 0;
                static int app_count = 0;
                static int app_max = 0;
                static int app_start = 0;
                static char* search_bar_str_empy = "Search apps...";
                static uint32_t y_initial_pos = 0;
                static uint32_t x_initial_pos = 0;
                if(!y_initial_pos){
                    y_initial_pos = content_y + content_height;
                }
                if(!already_search){
                    already_search = true;

                    app_focus = 0;

                    if(apps_found != NULL){
                        free_app_url_by(apps_found);
                    }

                    if(search_with_tag){
                        apps_found = find_apps_url_by_tag(curl, (search_bar_str == NULL || search_bar_str == search_bar_str_empy) ? NULL : search_bar_str);
                    }else{
                        apps_found = find_apps_url_by_name(curl, (search_bar_str == NULL || search_bar_str == search_bar_str_empy) ? NULL : search_bar_str);
                    }
                    app_count = 0;
                    app_start = 0;
                    if(apps_found != NULL){
                        while(apps_found[app_count] != NULL){
                            app_count++;
                        }
                    }
                    draw_content_box = true;
                }
                if(draw_search_box){
                    draw_rectangle(&fb, content_x, content_y, content_width, y_initial_pos - content_y, ~TEXT_COLOR);

                    draw_search_box = false;
                    if(search_bar_str == NULL || search_bar_str == search_bar_str_empy){
                        set_pen_color(font, SEARCH_BAR_COLOR);
                        search_bar_str = search_bar_str_empy;
                    }else{
                        set_pen_color(font, TEXT_COLOR);
                    }

                    set_pen_size(font, SEARCH_BAR_SIZE);
                    write_paragraph(font, content_x, content_y, content_width, PARAGRAPH_CENTER, search_bar_str, 1);
                    y_initial_pos = get_pen_pos_y(font) + HEADER_MARGIN;
                    x_initial_pos = content_x + HEADER_MARGIN;
                    draw_rectangle_border(&fb, x_initial_pos, y_initial_pos, content_width - (HEADER_MARGIN * 2), 1, TEXT_COLOR);
                    y_initial_pos += 1;
                }

                if(draw_content_box){
                    draw_rectangle(&fb, content_x, y_initial_pos, content_width, content_height - (y_initial_pos - content_y), ~TEXT_COLOR);

                    uint32_t x_pos = x_initial_pos;
                    uint32_t y_pos = y_initial_pos;
                    draw_content_box = false;
                    char* search_info;
                    assert(asprintf(&search_info, "%d %s found | Press <Enter> to refresh | Press <i> to install | %s", app_count, app_count ? "apps" : "app", search_with_tag ? "Search by Tag (change to Name with <TAB>)" : "Search by Name (change to Tag with <TAB>)") >= 0);
                    set_pen_color(font, TEXT_COLOR);
                    set_pen_size(font, TEXT_SIZE);
                    write_paragraph(font, x_pos, y_pos, content_width, PARAGRAPH_LEFT, search_info, 1);

                    y_pos = get_pen_pos_y(font) + HEADER_MARGIN;
                    draw_rectangle_border(&fb, x_pos, y_pos, content_width - (HEADER_MARGIN * 2), 1, TEXT_COLOR);

                    app_max = (content_height - get_pen_pos_y(font)) / (TITLE_SIZE + TEXT_SIZE + HEADER_MARGIN);
                    
                    for(int i = app_start; i < app_count && i < app_start + app_max; i++){
                        uint32_t current_color = (app_focus == i) ? FOCUS_COLOR : TEXT_COLOR;
                        set_pen_color(font, current_color);
                        set_pen_size(font, TITLE_SIZE);
                        write_paragraph(font, x_pos, y_pos, content_width, PARAGRAPH_LEFT, apps_found[i]->name, 1);
                        y_pos = get_pen_pos_y(font);
                        set_pen_size(font, TEXT_SIZE);
                        write_paragraph(font, x_pos, y_pos, content_width, PARAGRAPH_LEFT, apps_found[i]->description, 1);
                        y_pos = get_pen_pos_y(font) + HEADER_MARGIN;
                        draw_rectangle_border(&fb, x_pos, y_pos, content_width - (HEADER_MARGIN * 2), 1, current_color);
                    }
                }

                int is_pressed = 0;
                uint64_t key = 0;
                uint16_t translated_key = 0;
                int is_key = get_key(&is_pressed, &key, &translated_key);

                if(is_key){
                    static bool arrow_pressed = false;
                    static bool wait_release = false;
                    static bool wait_release_up = false;
                    static bool wait_release_down = false;
                    static bool wait_release_right = false;
                    static bool wait_release_left = false;

                    if(key == 96 && !arrow_pressed){
                            arrow_pressed = true;
                    }else if(arrow_pressed && is_pressed){
                        if(key == 72 && !wait_release_up){
                            // up
                            wait_release_up = true;
                            if(app_focus > 0){
                                app_focus--;
                                if(app_start != 0 && app_focus == app_start){
                                    app_start--;
                                }
                                draw_content_box = true;
                            }
                        }else if(key == 80 && !wait_release_down){
                            // down
                            wait_release_down = true;
                            if(app_focus < app_count){
                                app_focus++;
                                if(app_focus == app_start + app_max - 1){
                                    app_start++;
                                }
                                draw_content_box = true;
                            }
                        }else if(key == 77 && !wait_release_right){
                            // right
                            wait_release_right = true;
                        }else if(key == 75 && !wait_release_left){
                            // left
                            wait_release_left = true;
                        }                            
                    }else if(is_pressed){
                        if(key == 1){
                            exit(EXIT_SUCCESS);
                        }else if(key == 60){
                            current_task = 1;
                            update_header = true;
                        }else if(key == 61){
                            current_task = 2;
                            update_header = true;
                        }else if(translated_key == '\n'){
                            draw_rectangle(&fb, content_x, y_initial_pos, content_width, content_height - (y_initial_pos - content_y), ~TEXT_COLOR);
                            set_pen_color(font, TEXT_COLOR);
                            set_pen_size(font, TEXT_SIZE);
                            write_paragraph(font, x_initial_pos, y_initial_pos, content_width, PARAGRAPH_LEFT, "Loading...", 1);
                            draw_frame();
                            already_search = false;
                        }else if(translated_key == '\t'){
                            search_with_tag = !search_with_tag;
                            draw_content_box = true;
                        }else if(translated_key == '\b'){
                            if(search_bar_str != search_bar_str_empy){
                                char* new_search_bar_str = NULL;
                                int new_len = strlen(search_bar_str) - 1;
                                if(new_len > 0){
                                    assert(asprintf(&new_search_bar_str, "%.*s", new_len, search_bar_str) >= 0);
                                    free(search_bar_str);
                                    search_bar_str = new_search_bar_str;
                                }else{
                                    free(search_bar_str);
                                    search_bar_str = search_bar_str_empy;
                                }
                                draw_search_box = true;
                            }
                        }else{
                            if(strlen((search_bar_str != search_bar_str_empy) ? search_bar_str : "") < SEARCH_MAX_CHAR){
                                char* new_search_bar_str = NULL;
                                assert(asprintf(&new_search_bar_str, "%s%c", (search_bar_str != search_bar_str_empy) ? search_bar_str : "", translated_key) >= 0);

                                if(search_bar_str != search_bar_str_empy){
                                    free(search_bar_str);
                                }
                                search_bar_str = new_search_bar_str;
                                draw_search_box = true;
                            }
                        }
                    }else{
                        if(arrow_pressed){
                            if(key == 72 && wait_release_up){
                                // up
                                wait_release_up = false;
                            }else if(key == 80 && wait_release_down){
                                // down
                                wait_release_down = false;
                            }else if(key == 77 && wait_release_right){
                                // right
                                wait_release_right = false;
                            }else if(key == 75 && wait_release_left){
                                // left
                                wait_release_left = false;
                            }
                        }
                    }
                }
                
                break;
            }
        }
        draw_frame();
    }

    unload_fb();
    unload_curl();
    unload_json();
    unload_font_data();

    return EXIT_SUCCESS;
}

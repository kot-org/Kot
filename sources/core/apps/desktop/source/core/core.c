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

#define DATE_TIME_COLOR         (0xffffff)
#define DATE_TIME_BACKGROUND    (0x111111)
#define ICON_TEXT_BACKGROUND    (0x333333)
#define DATE_TIME_SIZE          (14)
#define DATE_TIME_HEIGHT        (26)
#define ICON_WIDTH              (75)
#define ICON_HEIGHT             (100)
#define ICON_IMAGE_WIDTH        (75)
#define ICON_IMAGE_HEIGHT       (75)
#define ICON_TEXT_SIZE          (18)
#define ICON_TEXT_COLOR         (0xffffff)
#define ICON_BORDER_COLOR       (0x3c2d2d)
#define ICON_BORDER_FOCUS_COLOR (0x14bacb)
#define ICON_BORDER_MARGIN      (5)
#define ICON_MARGIN             (20)

kfont_t font;

int fb_fd = -1;
kframebuffer_t fb;
struct fb_fix_screeninfo fix_screeninfo;
struct fb_var_screeninfo var_screeninfo;

raw_image_t* wallpaper_resized = NULL;

char* wallpaper_path = NULL;
char* font_path = NULL;

size_t json_size = 0;
void* json_buffer = NULL;
FILE* json_file = NULL;
cJSON* json_root = NULL;
cJSON* wallpaper_path_json = NULL;
cJSON* font_path_json = NULL;

int icon_row_count = 0;
int icon_column_count = 0;

int max_text_icon_length = 0;

int start_x = ICON_MARGIN;
int start_y = ICON_MARGIN + DATE_TIME_HEIGHT;

int current_page = 0;
int page_count = 0;

int icons_count = 0;
int icon_start_count = 0;
int focus_icon_row = 0;
int focus_icon_column = 0;
int icons_max_count = 0;
raw_image_t** icons_image = NULL;
char** icons_text = NULL;
char** icons_executable_path = NULL;
char** icons_cwd_path = NULL;

bool update_icon_page = true;
bool update_focus = true;

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

int load_fb(){
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

    return EXIT_SUCCESS;
}

int load_json(){
    json_file = fopen("/usr/bin/res/desktop/desktop.json", "r"); 

    if(json_file == NULL){ 
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

    return EXIT_SUCCESS;
}

int load_wallpaper(char* wallpaper_path){
    raw_image_t* wallpaper = load_jpeg_image_file(wallpaper_path);
    if(wallpaper == NULL){
        perror("error loading wallpaper\n");
        return EXIT_FAILURE;
    }
    if(wallpaper->width < wallpaper->height){
        wallpaper_resized = resize_image(wallpaper, 0, fb.height, true);
    }else{
        wallpaper_resized = resize_image(wallpaper, fb.width, 0, true);
    }

    free_raw_image(wallpaper);

    return EXIT_SUCCESS;
}

int load_font_data(char* font_path){
    FILE* font_file = fopen(font_path, "rb");

    if(font_file == NULL){
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

    return EXIT_SUCCESS;
}

int process_icons(const char* json_file_path){
    FILE* file = fopen(json_file_path, "r");
    if (!file) {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* data = malloc(length + 1);
    fread(data, 1, length, file);
    fclose(file);
    data[length] = '\0';

    cJSON* json = cJSON_Parse(data);
    free(data);

    if(!json){
        fprintf(stderr, "Error parsing JSON\n");
        return EXIT_FAILURE;
    }

    int c = 0;
    int total_count = 0;
    icons_text = malloc(sizeof(char*) * icons_max_count);
    icons_executable_path = malloc(sizeof(char*) * icons_max_count);
    icons_cwd_path = malloc(sizeof(char*) * icons_max_count);
    icons_image = malloc(sizeof(raw_image_t*) * icons_max_count);

    cJSON* item = NULL;
    cJSON_ArrayForEach(item, json) {
        if (total_count < icon_start_count) {
            total_count++;
        } else if (total_count < icons_max_count + icon_start_count) {
            cJSON* iconPath = cJSON_GetObjectItem(item, "iconPath");
            cJSON* executablePath = cJSON_GetObjectItem(item, "executablePath");
            cJSON* cwdPath = cJSON_GetObjectItem(item, "cwdPath");
            cJSON* appName = cJSON_GetObjectItem(item, "appName");

            if(iconPath && executablePath && cwdPath && appName) {
                raw_image_t* icon = load_tga_image_file(iconPath->valuestring);

                if (icon != NULL) {
                    raw_image_t* icon_resized = NULL;
                    if (icon->width < icon->height) {
                        icon_resized = resize_image(icon, 0, ICON_IMAGE_HEIGHT, true);
                    } else {
                        icon_resized = resize_image(icon, ICON_IMAGE_WIDTH, 0, true);
                    }

                    free_raw_image(icon);

                    if(icon_resized != NULL){
                        size_t name_length = strlen(appName->valuestring) + 1;
                        char* name = malloc(name_length);
                        strncpy(name, appName->valuestring, name_length);

                        icons_image[c] = icon_resized;
                        icons_text[c] = name;
                        icons_executable_path[c] = strdup(executablePath->valuestring);
                        icons_cwd_path[c] = strdup(cwdPath->valuestring);

                        if (name_length > max_text_icon_length) {
                            char* correct_name = malloc(name_length);
                            strncpy(correct_name, name, name_length);
                            icons_executable_path[c] = correct_name;

                            name[max_text_icon_length - 3] = '.';
                            name[max_text_icon_length - 2] = '.';
                            name[max_text_icon_length - 1] = '.';
                            name[max_text_icon_length] = '\0';
                        }

                        c++;
                        total_count++;
                    }
                }
            }
        } else {
            total_count++;
        }
    }

    cJSON_Delete(json);

    icons_count = c;
    page_count = DIV_ROUND_UP(c, icons_max_count);

    return EXIT_SUCCESS;
}

void get_input(){
    static bool arrow_pressed = false;
    static bool wait_release = false;
    static bool wait_release_up = false;
    static bool wait_release_down = false;
    static bool wait_release_right = false;
    static bool wait_release_left = false;
    
    int pressed = 0;
    uint64_t key = 0;
    int ret = get_key(&pressed, &key);

    if(ret){
        if(key == 96 && !arrow_pressed){
            arrow_pressed = true;
        }else{
            if(arrow_pressed && pressed){
                uint32_t x = start_x + focus_icon_column * (ICON_MARGIN + ICON_WIDTH);
                uint32_t y = start_y + focus_icon_row * (ICON_MARGIN + ICON_HEIGHT);
                draw_rectangle_border(&fb, x - ICON_BORDER_MARGIN, y - ICON_BORDER_MARGIN, ICON_WIDTH + 2 * ICON_BORDER_MARGIN, ICON_HEIGHT + 2 * ICON_BORDER_MARGIN, ICON_BORDER_COLOR);

                if(key == 72 && !wait_release_up){
                    // up
                    wait_release_up = true;
                    if(focus_icon_row > 0){
                        focus_icon_row--;
                    }
                }else if(key == 80 && !wait_release_down){
                    // down
                    wait_release_down = true;
                    if(((focus_icon_column + 1) * (focus_icon_row + 2)) <= icons_count){
                        focus_icon_row++;
                    }
                }else if(key == 77 && !wait_release_right){
                    // right
                    wait_release_right = true;
                    if(((focus_icon_column + 2) * (focus_icon_row + 1)) <= icons_count){
                        focus_icon_column++;
                    }
                }else if(key == 75 && !wait_release_left){
                    // left
                    wait_release_left = true;
                    if(focus_icon_column > 0){
                        focus_icon_column--;
                    }
                }

                update_focus = true;
                arrow_pressed = false;
            }else if(arrow_pressed && !pressed){
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
                arrow_pressed = false;
            }else if(key == 28 && pressed){
                int c = focus_icon_column + focus_icon_row * icon_column_count;

                {
                    pid_t p = fork(); 
                    if(p < 0){ 
                        perror("desktop: fork failed"); 
                    }else if(p == 0){
                        char* exe_argv[2] = {icons_executable_path[c], NULL};
                        chdir(icons_cwd_path[c]);
                        execvp(icons_executable_path[c], exe_argv);
                        
                        printf("\033[1;31m%s : \033[0m", icons_executable_path[c]);
                        perror("\033[1;31mdesktop: launching failed\033[0m"); 
                        printf("\033[1;31mPress <Enter> to Continue\033[0m\n");
                        getchar();
                        exit(EXIT_FAILURE);
                    }else{
                        int status;
                        wait(&status);

                        /* reset display info */
                        assert(ioctl(fb_fd, FBIOPUT_VSCREENINFO, &var_screeninfo) == 0);
                    }
                }
            }
        }
    }
}

void draw_desktop(){
    char date_time_str[20];

    if(update_icon_page){
        draw_image(&fb, wallpaper_resized, 0, 0, fb.width, fb.height);
    }

    draw_rectangle(&fb, 0, 0, fb.width, DATE_TIME_HEIGHT, DATE_TIME_BACKGROUND);

    get_current_date_time(date_time_str);

    load_pen(font, &fb, 0, 0, DATE_TIME_SIZE, 0, DATE_TIME_COLOR);
    write_paragraph(font, 0, 0, fb.width, PARAGRAPH_CENTER, date_time_str);


    char page_string[20];
    snprintf(page_string, 20, "Page : %d / %d", current_page + 1, page_count);
    write_paragraph(font, 0, 0, fb.width - 25, PARAGRAPH_RIGHT, page_string);
    
    if(update_icon_page){
        update_icon_page = false;
        update_focus = false;

        int c = 0;
        for(int j = 0; j < icon_row_count; j++){
            for(int i = 0; i < icon_column_count; i++){
                if(c >= icons_count){
                    break;
                }

                uint32_t x = start_x + i * (ICON_MARGIN + ICON_WIDTH);
                uint32_t y = start_y + j * (ICON_MARGIN + ICON_HEIGHT);
                draw_image_with_binary_transparency(&fb, icons_image[c], x, y, ICON_IMAGE_WIDTH, ICON_IMAGE_HEIGHT);
                
                if(i == focus_icon_column && j == focus_icon_row){
                    draw_rectangle_border(&fb, x - ICON_BORDER_MARGIN, y - ICON_BORDER_MARGIN, ICON_WIDTH + 2 * ICON_BORDER_MARGIN, ICON_HEIGHT + 2 * ICON_BORDER_MARGIN, ICON_BORDER_FOCUS_COLOR);
                }else{
                    draw_rectangle_border(&fb, x - ICON_BORDER_MARGIN, y - ICON_BORDER_MARGIN, ICON_WIDTH + 2 * ICON_BORDER_MARGIN, ICON_HEIGHT + 2 * ICON_BORDER_MARGIN, ICON_BORDER_COLOR);
                }

                set_pen_size(font, ICON_TEXT_SIZE);
                set_pen_color(font, ~ICON_TEXT_COLOR);
                write_paragraph(font, x + 1, y + ICON_IMAGE_HEIGHT + 1, ICON_IMAGE_WIDTH, PARAGRAPH_CENTER, icons_text[c]);
                set_pen_color(font, ICON_TEXT_COLOR);
                write_paragraph(font, x, y + ICON_IMAGE_HEIGHT, ICON_IMAGE_WIDTH, PARAGRAPH_CENTER, icons_text[c]);

                c++;
            }
        }
    }

    if(update_focus){
        update_focus = false;
        uint32_t x = start_x + focus_icon_column * (ICON_MARGIN + ICON_WIDTH);
        uint32_t y = start_y + focus_icon_row * (ICON_MARGIN + ICON_HEIGHT);
        draw_rectangle_border(&fb, x - ICON_BORDER_MARGIN, y - ICON_BORDER_MARGIN, ICON_WIDTH + 2 * ICON_BORDER_MARGIN, ICON_HEIGHT + 2 * ICON_BORDER_MARGIN, ICON_BORDER_FOCUS_COLOR);
    }

    draw_frame();

    get_input();
}

int main(int argc, char* argv[]){
    if(load_fb() != EXIT_SUCCESS){
        return EXIT_FAILURE;
    }

    if(load_json() != EXIT_SUCCESS){
        return EXIT_FAILURE;
    }

    if(load_wallpaper(wallpaper_path) != EXIT_SUCCESS){
        fclose(json_file);

        return EXIT_FAILURE;
    }

    if(load_font_data(font_path) != EXIT_SUCCESS){
        free_raw_image(wallpaper_resized);

        fclose(json_file);

        return EXIT_FAILURE;
    }

    icon_column_count = fb.width / (ICON_WIDTH + ICON_MARGIN);
    icon_row_count = (fb.height - DATE_TIME_HEIGHT) / (ICON_HEIGHT + ICON_MARGIN);
    start_x += (fb.width - (start_x + icon_column_count * (ICON_WIDTH + ICON_MARGIN))) / 2;
    start_y += (fb.height - (start_y + icon_row_count * (ICON_HEIGHT + ICON_MARGIN))) / 2;

    icons_max_count = icon_row_count * icon_column_count;

    kfont_pos_t char_width = 0;
    load_pen(font, &fb, 0, 0, ICON_TEXT_SIZE, 0, ICON_TEXT_COLOR);
    get_textbox_info(font, " ", &char_width, NULL, NULL, NULL);
    max_text_icon_length = ICON_IMAGE_WIDTH / char_width;

    icon_start_count = current_page * icons_max_count;

    process_icons("/usr/bin/icons/icons.json");

    while(true){
        draw_desktop();
    }

    free_raw_image(wallpaper_resized);

    fclose(json_file);

    return EXIT_FAILURE;
}
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

#define TEXT_COLOR         (0xffffff)
#define WRONG_COLOR        (0xff4633)
#define HOUR_SIZE          (52)
#define DATE_SIZE          (22)
#define INFO_SIZE          (16)
#define PIN_SIZE           (52)
#define PIN_INFO_SIZE      (22)

kfont_t font;

bool redraw_image = true;

kfont_pos_t pin_x = 0;
kfont_pos_t pin_y = 0;

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
cJSON* pin_expected_json = NULL;

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

void get_current_time(char *time_str) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(time_str, 6, "%H:%M", t);
}

void get_current_date(char *date_str) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(date_str, 30, "%A, %B %d", t);
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

int wait_enter(){
    int pressed;
    uint64_t key;

    if(get_key(&pressed, &key)){
        if(pressed && key == 28){
            return 1;
        }
    }
    return 0;
}

int wait_for_pin(char* display_pin, char* pin, char* pin_expected, bool is_pin_set){
    int pressed;
    uint64_t key;

    if(get_key(&pressed, &key)){
        if(pressed && key == 1){
            return 2;
        }else if(pressed && key >= 1 && key <= 11){
            int number = key - 1;
            if(number == 10){
                number = 0;
            }
            char* str_pin = pin;
            int c = 0;
            while(*str_pin){
                str_pin++;
                c++;
            }

            str_pin[0] = (char)('0' + number);
            str_pin[1] = (char)('\0');
            display_pin[c * 2] = '*';

            if(c == 3){
                if(is_pin_set){
                    return 1;
                }else{
                    cJSON_SetValuestring(pin_expected_json, pin);

                    json_buffer = cJSON_Print(json_root);
                    json_size = strlen(json_buffer) + 1;

                    fseek(json_file, 0, SEEK_SET);
                    fwrite(json_buffer, 1, json_size, json_file);
                    return 4;
                }
            }
        }else if(pressed && key == 14){
            int number = key - 1;
            if(number == 10){
                number = 0;
            }
            char* str_pin = pin;
            int c = 0;
            while(*str_pin){
                str_pin++;
                c++;
            }
            str_pin--;
            c--;

            str_pin[0] = (char)('\0');
            display_pin[c * 2] = '_';  
            redraw_image = true;          
        }
    }

    if(is_pin_set){
        return 0;
    }else{
        return 3;
    }
}

int check_pin(char* pin, char* pin_expected){
    // todo encrypt pin
    return (!strcmp(pin, pin_expected));
}

int main(int argc, char* argv[]){
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
        perror("'/dev/fb0' : format not supported\n");
        return EXIT_FAILURE;
    }

    json_file = fopen("/usr/bin/res/lock/lock.json", "r+");

    if(json_file == NULL){ 
        fclose(json_file);

        perror("Error: Unable to open the file : /usr/bin/res/lock/lock.json\n"); 
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
    char* pin_expected = NULL;

    if(json_root != NULL){ 
        wallpaper_path_json = cJSON_GetObjectItem(json_root, "wallpaper_path");
        if(cJSON_IsString(wallpaper_path_json) && (wallpaper_path_json->valuestring != NULL)){
            wallpaper_path = malloc(strlen(wallpaper_path_json->valuestring) + 1);
            strcpy(wallpaper_path, wallpaper_path_json->valuestring);
        }else{
            fclose(json_file);

            printf("Error: Unable to parse the file : /usr/bin/res/lock/lock.json\n"); 
            return EXIT_FAILURE; 
        }
        
        font_path_json = cJSON_GetObjectItem(json_root, "font_path");
        if(cJSON_IsString(font_path_json) && (font_path_json->valuestring != NULL)){
            font_path = malloc(strlen(font_path_json->valuestring) + 1);
            strcpy(font_path, font_path_json->valuestring);
        }else{
            fclose(json_file);
            
            printf("Error: Unable to parse the file : /usr/bin/res/lock/lock.json\n"); 
            return EXIT_FAILURE; 
        }

        pin_expected_json = cJSON_GetObjectItem(json_root, "pin_expected");
        if(cJSON_IsString(pin_expected_json) && (pin_expected_json->valuestring != NULL)){
            pin_expected = malloc(strlen(pin_expected_json->valuestring) + 1);
            strcpy(pin_expected, pin_expected_json->valuestring);
        }else{
            fclose(json_file);

            printf("Error: Unable to parse the file : /usr/bin/res/lock/lock.json\n"); 
            return EXIT_FAILURE; 
        }
    }else{
        fclose(json_file);

        printf("Error: Unable to parse the file : /usr/bin/res/lock/lock.json\n"); 
        return EXIT_FAILURE; 
    }

    bool is_pin_set = false;

    if(strlen(pin_expected) == 4){
        is_pin_set = true;
    }

    raw_image_t* wallpaper = load_jpeg_image_file(wallpaper_path);

    if(wallpaper == NULL){
        fclose(json_file);

        perror("error loading wallpaper\n");
        return EXIT_FAILURE;
    }

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

    char* display_pin = "_ _ _ _";
    char* textbox_display_pin = strdup(display_pin);
    char pin[5];
    char ask_pin[48];
    strcpy(ask_pin, "Please enter your PIN :");
    color_t pin_color = TEXT_COLOR;
    pin[0] = '\0';

    while(true){
        while(true){
            char time_str[6];
            char date_str[30];

            draw_image(&fb, wallpaper_resized, 0, 0, fb.width, fb.height);

            get_current_time(time_str);
            get_current_date(date_str);

            load_pen(font, &fb, 0, 0, HOUR_SIZE, 0, TEXT_COLOR);
            write_paragraph(font, -1, -1, fb.width, PARAGRAPH_CENTER, time_str);

            set_pen_size(font, DATE_SIZE);
            write_paragraph(font, -1, -1, fb.width, PARAGRAPH_CENTER, date_str);

            set_pen_size(font, INFO_SIZE);
            write_paragraph(font, -1, fb.height - INFO_SIZE - 50, fb.width, PARAGRAPH_CENTER, "Press <Enter> to log in\n");

            draw_frame();

            if(wait_enter()){
                break;
            }
        }

        while(true){
            char time_str[6];
            char date_str[20];

            int r = wait_for_pin(display_pin, pin, pin_expected, is_pin_set);
            if(r == 1){
                if(check_pin(pin, pin_expected)){
                    free_raw_image(wallpaper_resized);

                    fclose(json_file);

                    return EXIT_SUCCESS;
                }
                pin[0] = '\0';
                strcpy(display_pin, textbox_display_pin);
                strcpy(ask_pin, "Wrong PIN, please enter your PIN :");
                pin_color = WRONG_COLOR;
                redraw_image = true;
            }else if(r == 2){
                break;
            }else if(r == 3){
                strcpy(ask_pin, "It seems you are new. Please choose your PIN :");
            }else if(r == 4){
                free_raw_image(wallpaper_resized);

                fclose(json_file);

                return EXIT_SUCCESS;
            }

            if(redraw_image){
                redraw_image = false;

                draw_image(&fb, wallpaper_resized, 0, 0, fb.width, fb.height);

                set_pen_size(font, PIN_INFO_SIZE);
                set_pen_color(font, pin_color);
                write_paragraph(font, -1, (fb.height - PIN_SIZE) / 2, fb.width, PARAGRAPH_CENTER, ask_pin);

                set_pen_color(font, TEXT_COLOR);
                set_pen_size(font, PIN_SIZE);
                pin_x = get_pen_pos_x(font);
                pin_y = get_pen_pos_y(font);
                write_paragraph(font, pin_x, pin_y, fb.width, PARAGRAPH_CENTER, textbox_display_pin);

                set_pen_size(font, INFO_SIZE);
                write_paragraph(font, -1, fb.height - INFO_SIZE - 50, fb.width, PARAGRAPH_CENTER, "Back : <Esc>\n");
            }

            set_pen_size(font, PIN_SIZE);
            write_paragraph(font, pin_x, pin_y, fb.width, PARAGRAPH_CENTER, display_pin);

            draw_frame();
        }
    }

    free_raw_image(wallpaper_resized);

    fclose(json_file);

    return EXIT_FAILURE;
}
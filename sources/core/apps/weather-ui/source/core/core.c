#include <stdio.h>
#include <fcntl.h>
#include <ctype.h> 
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include <linux/fb.h>
#include <sys/ioctl.h>

#include <cjson/cJSON.h>

#include <kot-graphics/font.h>
#include <kot-graphics/utils.h>
#include <kot-graphics/image.h>

#define TEXT_COLOR         (0xffffff)
#define HOUR_SIZE          (52)
#define DATE_SIZE          (22)
#define CITY_SIZE          (32)
#define TEMP_SIZE          (49)
#define WIND_SIZE          (22)
#define INFO_SIZE          (16)

kfont_t font;

char* font_path = NULL;
char* wallpaper_path = NULL;
FILE* json_file = NULL;
raw_image_t* wallpaper_resized = NULL;

cJSON* weather_json = NULL;

int fb_fd = -1;
kframebuffer_t fb;
struct fb_fix_screeninfo fix_screeninfo;
struct fb_var_screeninfo var_screeninfo;


char* area_name_str = NULL;
char* temp_c_str = NULL;
char* desc_str = NULL;
char* wind_speed_str = NULL;

typedef struct{
    char* buffer;
    size_t size;
}curl_data_t;

size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp){
    size_t real_size = size * nmemb;
    curl_data_t* data = (curl_data_t*)userp;

    char* ptr = (char*)realloc(data->buffer, data->size + real_size + 1);
    if(ptr == NULL){
        printf("Not enough memory (realloc returned NULL)\n");
        return 0;
    }

    data->buffer = ptr;
    memcpy(&(data->buffer[data->size]), contents, real_size);
    data->size += real_size;
    data->buffer[data->size] = 0;

    return real_size;
}

char* get_weather_data(const char* city){
    CURL* curl;
    CURLcode res;
    curl_data_t data = {NULL, 0};

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl){
        char url[256];
        snprintf(url, sizeof(url), "https://wttr.in/%s?format=j1", city);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_CAINFO, "/usr/etc/ssl/cert.pem");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&data);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK){
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return data.buffer;
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
    strftime(date_str, 20, "%A, %B %d", t);
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

void draw_frame(){
    write(fb_fd, fb.buffer, fb.size);
}

void convert_name_to_path(char* name){
    while(*name){
        if(*name == ' '){
            *name = '_';
        }
        *name = tolower(*name);
        name++;
    }
}

int load_fb(){
    fb_fd = open("/dev/fb0", O_RDWR);

    printf("Loading weather-ui...\n");

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
    json_file = fopen("/usr/bin/res/weather-ui/weather-ui.json", "r+");
    if(json_file == NULL){
        perror("Error: Unable to open the file : /usr/bin/res/weather-ui/weather-ui.json");
        close(fb_fd);
        return EXIT_FAILURE;
    }

    fseek(json_file, 0, SEEK_END);
    size_t json_size = ftell(json_file);
    fseek(json_file, 0, SEEK_SET);

    void* json_buffer = malloc(json_size);
    fread(json_buffer, 1, json_size, json_file);

    cJSON* json_root = cJSON_Parse(json_buffer);
    free(json_buffer);

    if(json_root == NULL){
        fprintf(stderr, "Failed to parse weather-ui.json");
        fclose(json_file);
        close(fb_fd);
        return EXIT_FAILURE;
    }

    cJSON* font_path_json = cJSON_GetObjectItem(json_root, "font_path");
    if(cJSON_IsString(font_path_json) && (font_path_json->valuestring != NULL)){
        font_path = strdup(font_path_json->valuestring);
    }else{
        fprintf(stderr, "Error: Unable to parse font_path from weather-ui.json\n");
        fclose(json_file);
        close(fb_fd);
        return EXIT_FAILURE;
    }

    cJSON* wallpaper_path_json = cJSON_GetObjectItem(json_root, "wallpaper_path");
    if(cJSON_IsString(wallpaper_path_json) && (wallpaper_path_json->valuestring != NULL)){
        wallpaper_path = strdup(wallpaper_path_json->valuestring);
    }else{
        fprintf(stderr, "Error: Unable to parse wallpaper_path from weather-ui.json\n");
        fclose(json_file);
        close(fb_fd);
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

int load_wallpaper(char* name){
    convert_name_to_path(name);

    char* real_wallpaper_path = NULL;
    asprintf(&real_wallpaper_path, "%s%s.jpg", wallpaper_path, name);
    if(real_wallpaper_path == NULL){
        perror("error loading wallpaper\n");
        fclose(json_file);
        close(fb_fd);
        return EXIT_FAILURE;
    }

    raw_image_t* wallpaper = load_jpeg_image_file(real_wallpaper_path);
    free(real_wallpaper_path);

    if(wallpaper == NULL){
        real_wallpaper_path = NULL;
        asprintf(&real_wallpaper_path, "%sdefault_wallpaper.jpg", wallpaper_path);
        wallpaper = load_jpeg_image_file(real_wallpaper_path);
        free(real_wallpaper_path);

        if(wallpaper == NULL){
            perror("error loading wallpaper\n");
            fclose(json_file);
            close(fb_fd);
            return EXIT_FAILURE;
        }
    }

    if(wallpaper->width < wallpaper->height){
        wallpaper_resized = resize_image(wallpaper, 0, fb.height, true);
    }else{
        wallpaper_resized = resize_image(wallpaper, fb.width, 0, true);
    }

    free_raw_image(wallpaper);

    return EXIT_SUCCESS;
}

int load_weather_info(){
    char* weather_data = get_weather_data("");
    weather_json = cJSON_Parse(weather_data);
    free(weather_data);

    if(weather_json == NULL){
        fprintf(stderr, "Failed to parse weather data\n");
        fclose(json_file);
        close(fb_fd);
        return EXIT_FAILURE;
    }

    cJSON* nearest_area = cJSON_GetObjectItem(weather_json, "nearest_area");
    if(nearest_area && cJSON_IsArray(nearest_area) && cJSON_GetArraySize(nearest_area) > 0){
        cJSON* area = cJSON_GetArrayItem(nearest_area, 0);
        if(area && cJSON_IsObject(area)){
            cJSON* area_name = cJSON_GetObjectItem(area, "areaName");
            if(area_name && cJSON_IsArray(area_name) && cJSON_GetArraySize(area_name) > 0){
                cJSON* area_first_name = cJSON_GetArrayItem(area_name, 0);
                if(area_first_name && cJSON_IsObject(area_first_name)){
                    cJSON* area_first_name_value = cJSON_GetObjectItem(area_first_name, "value");
                    if(area_first_name_value && cJSON_IsString(area_first_name_value)){
                        area_name_str = area_first_name_value->valuestring;
                    }
                }
            }
        }
    }

    cJSON* current_condition = cJSON_GetObjectItem(weather_json, "current_condition");
    if(current_condition && cJSON_IsArray(current_condition) && cJSON_GetArraySize(current_condition) > 0){
        cJSON* condition = cJSON_GetArrayItem(current_condition, 0);
        if(condition && cJSON_IsObject(condition)){
            cJSON* temp_c = cJSON_GetObjectItem(condition, "temp_C");
            cJSON* wind_speed_kmph = cJSON_GetObjectItem(condition, "windspeedKmph");
            cJSON* weather_desc = cJSON_GetObjectItem(condition, "weatherDesc");
            if(temp_c && cJSON_IsString(temp_c) && wind_speed_kmph && cJSON_IsString(wind_speed_kmph) && weather_desc && cJSON_IsArray(weather_desc) && cJSON_GetArraySize(weather_desc) > 0){
                cJSON* desc = cJSON_GetArrayItem(weather_desc, 0);
                if(desc && cJSON_IsObject(desc)){
                    cJSON* desc_value = cJSON_GetObjectItem(desc, "value");
                    if(desc_value && cJSON_IsString(desc_value)){
                        temp_c_str = temp_c->valuestring;
                        desc_str = desc_value->valuestring;
                        wind_speed_str = wind_speed_kmph->valuestring;
                    }
                }
            }
        }
    }

    if(load_wallpaper(desc_str) != EXIT_SUCCESS){
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int draw_ui(){
    draw_image(&fb, wallpaper_resized, 0, 0, fb.width, fb.height);

    char time_str[6];
    char date_str[20];

    get_current_time(time_str);
    get_current_date(date_str);

    load_pen(font, &fb, 0, 10, HOUR_SIZE, 0, TEXT_COLOR);

    kfont_pos_t x = get_pen_pos_x(font);
    kfont_pos_t y = get_pen_pos_y(font);
    set_pen_color(font, ~TEXT_COLOR);
    write_paragraph(font, -1, -1, fb.width, PARAGRAPH_CENTER, time_str);
    set_pen_color(font, TEXT_COLOR);
    write_paragraph(font, x - 1, y - 1, fb.width, PARAGRAPH_CENTER, time_str);

    set_pen_size(font, DATE_SIZE);
    x = get_pen_pos_x(font);
    y = get_pen_pos_y(font);
    set_pen_color(font, ~TEXT_COLOR);
    write_paragraph(font, -1, -1, fb.width, PARAGRAPH_CENTER, date_str);
    set_pen_color(font, TEXT_COLOR);
    write_paragraph(font, x - 1, y - 1, fb.width, PARAGRAPH_CENTER, date_str);

    set_pen_pos_y(font, (fb.height - (TEMP_SIZE + WIND_SIZE + CITY_SIZE)) / 2);
    set_pen_size(font, CITY_SIZE);
    x = get_pen_pos_x(font);
    y = get_pen_pos_y(font);
    set_pen_color(font, ~TEXT_COLOR);
    write_paragraph(font, -1, -1, fb.width, PARAGRAPH_CENTER, area_name_str);
    set_pen_color(font, TEXT_COLOR);
    write_paragraph(font, x - 1, y - 1, fb.width, PARAGRAPH_CENTER, area_name_str);


    char weather_info[100];

    snprintf(weather_info, sizeof(weather_info), "%s%cC, %s", temp_c_str, 0xb0, desc_str);
    set_pen_size(font, TEMP_SIZE);
    x = get_pen_pos_x(font);
    y = get_pen_pos_y(font);
    set_pen_color(font, ~TEXT_COLOR);
    write_paragraph(font, -1, -1, fb.width, PARAGRAPH_CENTER, weather_info);
    set_pen_color(font, TEXT_COLOR);
    write_paragraph(font, x - 1, y - 1, fb.width, PARAGRAPH_CENTER, weather_info);

    snprintf(weather_info, sizeof(weather_info), "\n%skm/h", wind_speed_str);
    set_pen_size(font, WIND_SIZE);
    x = get_pen_pos_x(font);
    y = get_pen_pos_y(font);
    set_pen_color(font, ~TEXT_COLOR);
    write_paragraph(font, -1, -1, fb.width, PARAGRAPH_CENTER, weather_info);
    set_pen_color(font, TEXT_COLOR);
    write_paragraph(font, x - 1, y - 1, fb.width, PARAGRAPH_CENTER, weather_info);

    set_pen_size(font, INFO_SIZE);
    x = get_pen_pos_x(font);
    y = fb.height - INFO_SIZE - 50;
    set_pen_color(font, ~TEXT_COLOR);
    write_paragraph(font, -1, fb.height - INFO_SIZE - 50, fb.width, PARAGRAPH_CENTER, "Exit : <Esc>\n");
    set_pen_color(font, TEXT_COLOR);
    write_paragraph(font, x - 1, y - 1, fb.width, PARAGRAPH_CENTER, "Exit : <Esc>\n");

    draw_frame();

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]){
    if(load_fb() != EXIT_SUCCESS){
        return EXIT_FAILURE;
    }

    if(load_json() != EXIT_SUCCESS){
        return EXIT_FAILURE;
    }

    if(load_font_data() != EXIT_SUCCESS){
        return EXIT_FAILURE;
    }

    if(load_weather_info() != EXIT_SUCCESS){
        return EXIT_FAILURE;
    }

    while(!wait_escape()){
        draw_ui();
    }

    cJSON_Delete(weather_json);
    free_raw_image(wallpaper_resized);
    fclose(json_file);
    close(fb_fd);

    return EXIT_SUCCESS;
}

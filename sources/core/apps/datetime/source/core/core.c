#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp){
    size_t real_size = size * nmemb;
    char *buf = (char*)userp;
    memcpy(&(buf[strlen(buf)]), contents, real_size);
    return real_size;
}

void print_help(){
    printf("Usage: ./program [--help | --credits | --utc]\n");
    printf("If no argument is provided, the program will fetch and display the current date and time in your timezone.\n");
}

void print_credits(){
    printf("A big thank you to worldtimeapi.org for providing the date and time data.\n");
}

void fetch_time(const char *url){
    CURL *curl;
    CURLcode res;
    char read_buffer[1024];
    memset(read_buffer, 0, sizeof(read_buffer));

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl){
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, read_buffer);
        curl_easy_setopt(curl, CURLOPT_CAINFO, "/usr/etc/ssl/cert.pem");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        res = curl_easy_perform(curl);

        if(res != CURLE_OK){
            printf("Error : curl_%s\n",  curl_easy_strerror(res));
        }else{
            char* datetime = strstr(read_buffer, "\"datetime\":\"");
            if (datetime) {
                datetime += 12;
                char *end = strstr(datetime, "\"");
                if (end) {
                    *end = '\0';
                    printf("Current date and time: %s\n", datetime);
                }
            }
        }

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

int main(int argc, char *argv[]){
    const char *url = "http://worldtimeapi.org/api/ip";

    if (argc == 2) {
        if (strcmp(argv[1], "--help") == 0) {
            print_help();
            return 0;
        }
        else if (strcmp(argv[1], "--credits") == 0) {
            print_credits();
            return 0;
        }
        else if (strcmp(argv[1], "--utc") == 0) {
            url = "http://worldtimeapi.org/api/timezone/UTC";
        }
    }

    fetch_time(url);
    return 0;
}

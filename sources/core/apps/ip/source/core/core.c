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
    printf("Usage: ./program [--help | --credits]\n");
    printf("If no argument is provided, the program will fetch and display your IP address.\n");
}

void print_credits(){
    printf("A big thank you to ifconfig.me for providing the IP data.\n");
}

int main(int argc, char *argv[]){
    CURL *curl;
    CURLcode res;
    char read_buffer[1024];
    memset(read_buffer, 0, sizeof(read_buffer));

    if(argc == 2){
        if(strcmp(argv[1], "--help") == 0){
            print_help();
            return 0;
        }else if (strcmp(argv[1], "--credits") == 0){
            print_credits();
            return 0;
        }
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl){
        curl_easy_setopt(curl, CURLOPT_URL, "ifconfig.me");
        curl_easy_setopt(curl, CURLOPT_CAINFO, "/usr/etc/ssl/cert.pem");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, read_buffer);
        res = curl_easy_perform(curl);

        if(res != CURLE_OK){
            printf("Error : curl_%s\n",  curl_easy_strerror(res));
        }else{
            printf("IP address: %s\n", read_buffer);
        }

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

void print_help(){
    printf("Usage: ./weather [location]\n");
    printf("If no location is provided, it will show the weather for your current location.\n");
    printf("For help: ./weather --help\n");
    printf("For credits: ./weather --credits\n");
}

void print_credits(){
    printf("A big thank you to wttr.in for providing the weather data.\n");
}

int main(int argc, char *argv[]){
    CURL *curl;
    CURLcode res;

    if(argc > 1){
        if(strcmp(argv[1], "--help") == 0){
            print_help();
            return 0;
        }

        if(strcmp(argv[1], "--credits") == 0){
            print_credits();
            return 0;
        }
    }

    curl = curl_easy_init();
    if(curl){
        char url[100];
        snprintf(url, sizeof(url), "wttr.in/%s?AFdT0", (argc > 1) ? argv[1] : "");

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_CAINFO, "/usr/etc/ssl/cert.pem");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        res = curl_easy_perform(curl);

        if(res != CURLE_OK){
            printf("Error : curl_%s\n",  curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }
    return 0;
}

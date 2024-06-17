#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <curl/curl.h>

static bool is_file_exists(char* path){
    struct stat sb;   
    return (stat(path, &sb) == 0);
}

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp){
    size_t real_size = size * nmemb;                                                                                   
    fwrite(contents, size, nmemb, (FILE*)userp);
    return real_size;
}

static int progress_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow){
    double time_begin = *(double*)clientp;

    int download_speed = dlnow / (time(NULL) - time_begin);
    printf("\033[-1B");
    if(download_speed > 0){
        int eta = (int)((dltotal - dlnow) / download_speed);
        printf("%d/%d bytes | ETA : %d s | Speed : %d kbit/s\n", (int)dlnow, (int)dltotal, eta, download_speed * 8 / 1000);
    }else{
        printf("%d/%d bytes | ETA : N/A s | Speed : N/A kbit/s\n", (int)dlnow, (int)dltotal);
    }

    return 0;
}


int download_file(CURL* curl, char* url, char* path, bool force_download){
    if(!is_file_exists(path) || force_download){
        printf("Initiating download of `%s` to `%s`\n", url, path);
        int r = -1;
        FILE* fp = fopen(path, "wb");

        if(fp == NULL){
            printf("Failed to open file `%s` for writing. Error: %s\n", path, strerror(errno));
        }else{
            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            curl_easy_setopt(curl, CURLOPT_CAINFO, "/usr/etc/ssl/cert.pem");
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

            double time_begin = time(NULL);
            curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &time_begin);
            curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);

            printf("N/A\n");
            CURLcode res = curl_easy_perform(curl);
            if(res == CURLE_OK){
                r = 0;
                printf("Completed successfully.\n");
            } else {
                printf("Failed. Error: %s\n", curl_easy_strerror(res));
            }

            fclose(fp);
        }

        return r;
    }else{
        return 0;
    }
}
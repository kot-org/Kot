#include <stdio.h>
#include <curl/curl.h>

int main(void){
    curl_global_init(CURL_GLOBAL_ALL);

    CURL* curl = curl_easy_init();

    if(curl){
        curl_easy_setopt(curl, CURLOPT_URL, "http://example.com/");

        CURLcode r = curl_easy_perform(curl);

        if(r != CURLE_OK){
            printf(stderr, "can't connect to 'http://example.com/' : %s\n", curl_easy_strerror(r));
        }

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return 0;
}
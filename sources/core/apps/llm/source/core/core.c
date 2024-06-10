#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    char **buffer = (char **)userp;
    size_t buffer_size = strlen(*buffer) + size * nmemb + 1;
    *buffer = realloc(*buffer, buffer_size);
    memcpy(&(*buffer)[strlen(*buffer)], contents, size * nmemb);
    (*buffer)[buffer_size - 1] = '\0';
    return size * nmemb;
}

void print_help()
{
    printf("Usage: ./llm\n");
    printf("For help: ./llm --help\n");
    printf("For credits: ./llm --credits\n");
}

void print_credits()
{
    printf("A big thank you to YOU for providing the API.\n");
}

int main(int argc, char *argv[])
{
    if (argc > 1){
        if (strcmp(argv[1], "--help") == 0){
            print_help();
            return 0;
        }

        if (strcmp(argv[1], "--credits") == 0){
            print_credits();
            return 0;
        }
    }

    if(argc != 1){
        print_help();
        return 1;
    }

    char question[1024];
    printf("Question: ");
    fgets(question, sizeof(question), stdin);
    question[strcspn(question, "\n")] = 0;
    printf("Answer: ");

    CURL *curl;
    CURLcode res;
    char *buffer = malloc(1);
    *buffer = '\0';

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl){
        size_t max_size = 215 + strlen(question);
        char* json_data = (char *)malloc(sizeof(char) * max_size);
        snprintf(json_data, max_size, "{\"model\": \"llama3-8b-instruct\", \"messages\": [{\"role\": \"system\", \"content\": \"You are a poetic assistant, skilled in explaining complex programming concepts with creative flair.\"}, {\"role\": \"user\", \"content\": \"%s\"}]}", question);

        curl_easy_setopt(curl, CURLOPT_URL, "127.0.0.1:5000/v1/chat/completions");
        curl_easy_setopt(curl, CURLOPT_CAINFO, "/usr/etc/ssl/cert.pem");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        res = curl_easy_perform(curl);

        if(res != CURLE_OK){
            printf("Error : curl_%s\n",  curl_easy_strerror(res));
        }
        else{
            char* content = strstr(buffer, "\"content\":\"");
            if(content){
                content += 11;
                char* end = strstr(content, "<|eot_id|>\"");
                if(end){
                    *end = '\0';
                    printf("%s\n", content);
                }
            }
        }

        curl_easy_cleanup(curl);
        free(json_data);
    }

    curl_global_cleanup();
    free(buffer);
    return 0;
}

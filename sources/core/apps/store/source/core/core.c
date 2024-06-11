#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#include "../apps/apps.h"

void print_help(){
    printf("For help: ./store --help\n");
    printf("To install an application: ./store --install [app name]\n");
}

int main(int argc, char *argv[]){
    CURL *curl;
    CURLcode res;

    if(argc > 1){
        if(!strcmp(argv[1], "--help")){
            print_help();
            return 0;
        }else if(!strcmp(argv[1], "--install") && argc == 2){
            char search_mode[3];
            printf("Search with > T(tag)/N(name)\n");
            fgets(search_mode, sizeof(search_mode), stdin);
            search_mode[strcspn(search_mode, "\n")] = 0;
            if(!strcmp("T", search_mode)){
                char tag[512];
                printf("What is the tag of the application you want to install?\n");
                fgets(tag, sizeof(tag), stdin);
                tag[strcspn(tag, "\n")] = 0;

                char** url = find_apps_url_by_tag(tag);

                if(url != NULL){
                    int i = 0;
                    while(url[i] != NULL){
                        printf("%d) %s\n", i, url[i]);
                        i++;
                    }

                    char link_index[10];
                    printf("Select the index you want to install :\n");
                    fgets(link_index, sizeof(link_index), stdin);
                    link_index[strcspn(link_index, "\n")] = 0;

                    int index_to_install = atoi(link_index);

                    if(index_to_install >= 0 && index_to_install < i){
                        char allow_install[3];
                        printf("%s > Would you like to install it? (Y/N)\n", url[index_to_install]);
                        fgets(allow_install, sizeof(allow_install), stdin);
                        allow_install[strcspn(allow_install, "\n")] = 0;
                        if(!strcmp("Y", allow_install)){
                            // TODO
                        }else{
                            printf("Cancel the installation\n");
                        }
                    }else{
                        printf("Unknow index !\n");
                    }

                    free(url);
                    return 0;
                }else{
                    printf("No application found with tag: %s. Please check the spelling or try a different tag.\n", tag);
                    return 0;
                }               
            }else if(!strcmp("N", search_mode)){
                char name[512];
                printf("What is the name of the application you want to install?\n");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;

                char* url = find_apps_url_by_name(name);

                if(url != NULL){
                    char allow_install[3];
                    printf("%s found in the store. Would you like to install it? (Y/N)\n", name);
                    fgets(allow_install, sizeof(allow_install), stdin);
                    allow_install[strcspn(allow_install, "\n")] = 0;
                    if(!strcmp("Y", allow_install)){
                        // TODO
                    }else{
                        printf("Cancel the installation\n");
                    }
                    free(url);
                    return 0;
                }else{
                    printf("Can't find %s in the store. Did you spell it correctly?\n", name);
                    return 0;
                }
            }else{
                printf("Unknow search method !\n");
                return 0;
            }
        }else if(!strcmp(argv[1], "--install") && argc == 3){
            char* name = argv[2];

            char* url = find_apps_url_by_name(name);

            if(url != NULL){
                char allow_install[3];
                printf("%s found in the store. Would you like to install it? (Y/N)\n", name);
                fgets(allow_install, sizeof(allow_install), stdin);
                allow_install[strcspn(allow_install, "\n")] = 0;
                if(!strcmp("Y", allow_install)){
                    // TODO
                }else{
                    printf("Cancel the installation\n");
                }
                free(url);
                return 0;
            }else{
                printf("Can't find %s in the store. Did you spell it correctly?\n", name);
                return 0;
            }
        }else{
            print_help();
        }
    }else{
        print_help();
    }

    return 0;
}

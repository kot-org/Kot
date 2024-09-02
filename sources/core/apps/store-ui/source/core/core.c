#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <curl/curl.h>

#include "../apps/apps.h"
#include "../launch/launch.h"
#include "../update/update.h"
#include "../install/install.h"
#include "../remove/remove.h"

int main(int argc, char *argv[]){
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL *curl = curl_easy_init();

    if(curl == NULL){
        printf("Can't load curl");
        return -1;
    }else{
        if(!strcmp(argv[1], "--install") && argc == 2){
            char search_mode[3];
            printf("Search with > T(tag)/N(name)\n");
            fgets(search_mode, sizeof(search_mode), stdin);
            search_mode[strcspn(search_mode, "\n")] = 0;
            if(!strcmp("T", search_mode)){
                char tag[512];
                printf("What is the tag of the application you want to install?\n");
                fgets(tag, sizeof(tag), stdin);
                tag[strcspn(tag, "\n")] = 0;

                app_url_by_tag_t** apps_available = find_apps_url_by_tag(curl, tag);

                if(apps_available != NULL){
                    int i = 0;
                    while(apps_available[i] != NULL){
                        printf("%d) %s\n", i, apps_available[i]->name);
                        i++;
                    }

                    char link_index[10];
                    printf("Select the index you want to install :\n");
                    fgets(link_index, sizeof(link_index), stdin);
                    link_index[strcspn(link_index, "\n")] = 0;

                    int index_to_install = atoi(link_index);

                    if(index_to_install >= 0 && index_to_install < i){
                        char allow_install[3];
                        printf("%s > Would you like to install it? (Y/N)\n", apps_available[index_to_install]->name);
                        fgets(allow_install, sizeof(allow_install), stdin);
                        allow_install[strcspn(allow_install, "\n")] = 0;
                        if(!strcmp("Y", allow_install)){
                            install_app(curl, apps_available[index_to_install]->url, apps_available[index_to_install]->name, false);
                        }else{
                            printf("Cancel the installation\n");
                        }
                    }else{
                        printf("Unknow index !\n");
                    }

                    free_app_url_by_tag(apps_available);
                }else{
                    printf("No application found with tag: %s. Please check the spelling or try a different tag.\n", tag);
                }               
            }else if(!strcmp("N", search_mode)){
                char name[512];
                printf("What is the name of the application you want to install?\n");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;

                char* url = find_apps_url_by_name(curl, name);

                if(url != NULL){
                    char allow_install[3];
                    printf("%s found in the store. Would you like to install it? (Y/N)\n", name);
                    fgets(allow_install, sizeof(allow_install), stdin);
                    allow_install[strcspn(allow_install, "\n")] = 0;
                    if(!strcmp("Y", allow_install)){
                        install_app(curl, url, name, false);
                    }else{
                        printf("Cancel the installation\n");
                    }
                    free(url);
                }else{
                    printf("Can't find %s in the store. Did you spell it correctly?\n", name);
                }
            }else{
                printf("Unknow search method !\n");
            }
        }else if(!strcmp(argv[1], "--install") && argc == 3){
            char* name = argv[2];

            char* url = find_apps_url_by_name(curl, name);
            
            if(url != NULL){
                char allow_install[3];
                printf("%s found in the store. Would you like to install it? (Y/N)\n", name);
                fgets(allow_install, sizeof(allow_install), stdin);
                allow_install[strcspn(allow_install, "\n")] = 0;
                if(!strcmp("Y", allow_install)){
                    install_app(curl, url, name, false);
                }else{
                    printf("Cancel the installation\n");
                }
                free(url);
            }else{
                printf("Can't find %s in the store. Did you spell it correctly?\n", name);
            }
        }else if(!strcmp(argv[1], "--update") && argc == 3){
            char* name = argv[2];
            update_app(curl, name);
        }else if(!strcmp(argv[1], "--remove") && argc == 3){
            char* name = argv[2];
            remove_app(name, true);
        }else if(!strcmp(argv[1], "--launch") && argc >= 3){
            char* name = argv[2];
            // Keep the app name
            launch_app(name, argc - 2, &argv[2]);
        }
    }

    curl_easy_cleanup(curl);

    curl_global_cleanup();

    return 0;
}

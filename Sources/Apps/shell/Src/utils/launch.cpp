#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <kot/sys.h>
#include <kot/launch.h>
#include <kot/uisd/srvs/system.h>

#include "system.h"

int main(int argc, char* argv[]) {
    if(argc < 2){
        printf("Usage : launch <executable-path>\n");
        return EXIT_FAILURE;
    }

    char* Prefix = "d1:/bin/";
    size_t PathSize = strlen(Prefix) + strlen(argv[1]) + 1;
    char* Path = (char*)calloc(PathSize, sizeof(char));

    strcat(Path, Prefix);
    strcat(Path, argv[1]);

    char* Argv[] = {Path, NULL};
    char* Env[] = {NULL};

    if(kot_launch(Path, Argv, Env) != KSUCCESS){
        printf("File not found : '%s'\n", Path);
    }

    return EXIT_SUCCESS;
}
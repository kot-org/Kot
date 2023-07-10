#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

int main(int argc, char *argv[]) {
    char* args[argc + 3];

    char cwd[PATH_MAX];

    if(getcwd(cwd, sizeof(cwd)) != NULL) {
        args[0] = KOT_BASE_PATH"/Toolchain/local/bin/clang";
        args[1] = "--target=x86_64-kot";
        args[2] = "--sysroot="KOT_BASE_PATH"/Sysroot/usr";
        
        for(int i = 1; i < argc; i++){
            args[i + 2] = argv[i];
        }

        args[argc + 2] = NULL;

        return execvp(args[0], args);
    } else {
        return 1;
    }
}

#include <kot/cstring.h>

int strlen(char* p){
    int count = 0;
    while (*p != '\0') {
        count++;
        p++;
    }
    return count;
}

int strcmp(char* a, char* b){
    int alen = strlen(a);
    int blen = strlen(b);
    if(alen == blen){
        for(int i = 0 ; i < alen; i++){
            if(a[i] != b[i]){
                return false;
            }
        }
        return true;
    }else{
        return false;
    }
}

int strncmp(char* a, char* b, size_t len){
    for(int i = 0 ; i < len; i++){
        if(a[i] != b[i]){
            return false;
        }
    }
    return true;
}
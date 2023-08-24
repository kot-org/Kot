#ifndef _STRING_H
#define _STRING_H 1

#include <stddef.h>

static inline size_t strlen(const char *str) {
	register const char *s;
	for (s = str; *s; s++);
	return(s - str);
}

static inline int strcmp(const char *str1, const char *str2) {
    while (*str1 != '\0' && *str2 != '\0' && *str1 == *str2) {
        str1++;
        str2++;
    }
    
    if(*str1 == *str2) {
        return 0;
    }else if (*str1 < *str2) {
        return -1;
    }else {
        return 1;
    }
}

static inline int strncmp(const char *s1, const char *s2, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        if (s1[i] != s2[i]) {
            return (unsigned char)s1[i] - (unsigned char)s2[i];
        }
        if (s1[i] == '\0') {
            return 0;
        }
    }
    return 0;
}

static inline char* strstr(const char* a, const char* b){
    size_t n = strlen(b);
 
    while(*a){
        if(!strncmp(a, b, n)){
            return (char*)a;
        }
        a++;
    }
 
    return 0;
}

static inline char* strchr(const char* p, int ch){
	char c = ch;

	while(*p){
		if(*p == c){
			return (char*)p;
        }
        p++;
	}

    return NULL;
}

static inline char* strrchr(const char* p, int ch){
    const char* last_occurrence = NULL;

    while(*p){
        if(*p == ch){
            last_occurrence = p;
        }
        p++;
    }

    return (char*)last_occurrence;
}

static inline char to_lower(char c){
    if(c >= 'A' && c <= 'Z'){
        return c + 32;
    }

    return c;
}

static inline char to_upper(char c){
    if(c >= 'a' && c <= 'z'){
        return c - 32;
    }

    return c;
}

#endif // _STRING_H
#ifndef LIB_STRING_H
#define LIB_STRING_H 1

#include <stddef.h>
#include <limits.h>

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

static inline char* strcat(char* destination, const char* source){
    char* ptr = destination + strlen(destination);
 
    while(*source != '\0'){
        *ptr++ = *source++;
    }

    *ptr = '\0';

    return destination;
}

static inline char* strcpy(char* destination, const char* source){
    if(destination == NULL){
        return NULL;
    }
 
    char *ptr = destination;
 
    while(*source != '\0'){
        *destination = *source;
        destination++;
        source++;
    }
 
    *destination = '\0';
 
    return ptr;
}

static inline char* strncpy(char* destination, const char* source, size_t num){
    if(destination == NULL){
        return NULL;
    }
 
    char* ptr = destination;
 
    while(*source && num--){
        *destination = *source;
        destination++;
        source++;
    }
 
    *destination = '\0';
 
    return ptr;
}

static inline int atoi(const char* str)
{
    int sign = 1, base = 0, i = 0;
 
    while (str[i] == ' ') {
        i++;
    }
 
    if (str[i] == '-' || str[i] == '+') {
        sign = 1 - 2 * (str[i++] == '-');
    }
 
    while (str[i] >= '0' && str[i] <= '9') {
        if (base > INT_MAX / 10
            || (base == INT_MAX / 10 && str[i] - '0' > 7)) {
            if (sign == 1)
                return INT_MAX;
            else
                return INT_MIN;
        }
        base = 10 * base + (str[i++] - '0');
    }
    return base * sign;
}

static inline void reverse(char str[], int length) 
{
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

static inline char* itoa(int num, char* str, int base) 
{
    int i = 0;
    int isNegative = 0;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    if (num < 0 && base != 10) {
        isNegative = 1;
        num = -num;
    }

    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    if (isNegative && base == 10)
        str[i++] = '-';

    str[i] = '\0';

    reverse(str, i);

    return str;
}

#endif // LIB_STRING_H
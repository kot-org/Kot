#ifndef __STRING_H
#define __STRING_H

#include<sys/defs.h>

int strcmp( char* string1,  char* string2); /* to compare two strings */
int strncmp(char* string1,  char* string2, int n); /* to compare two strings till n */
char *strstr(char* big,  char* little);     /* to obtain substring */
char *strcpy(char* dest, const char* src);      /* to copy one string to another */
char *strncpy(char* dest, const char* src, int n);      /* to copy one string to another till n */

int strlen(const char* string);                       /* find length of string*/
int atoi(char *str);                            /*string to integer conversion*/
char* strtok(char *str, char* delim);     /*string tokenizer function*/
void* memset(void *ptr, int value, uint64_t num);
void *memcpy(void *dest, const void *src, uint64_t n);
int strcat(char *str1,char *str2);

#endif /* string_h */

//
//  string.c
//  sbush
//
//  Created by Banani on 9/3/17.
//  Copyright © 2017 Banani. All rights reserved.
//

#include <sys/string.h>


int strcmp(char *str1,char* str2)
{
        while(*str1 && * str2 && *str1==*str2)
        {
                str1++;
                str2++;
        }
        return *str1 - *str2;
}

char *strstr( char* big,  char* little){

    int little_len = strlen(little);

    while(*big){
        if(*big == *little){
            if(!strncmp(big, little, little_len))
                return (char*)big;
        }
        big++;
    }
    return NULL;

}


int  strlen(const char* string)
{
        int len = 0;
        while(*string++)
                len++;
        return len;
}


int strncmp(char* str1,char* str2, int n){
 	while(*str1 && * str2 && *str1==*str2 && n--)
        {
                str1++;
                str2++;
        }
        return (unsigned char)*str1 - (unsigned char)*str2;

}


char *strcpy(char* dest, const char* src){

    char* new_dest = dest;
    while(*src){
        *dest = *src;
        dest++; src++;

    }
    *dest ='\0';

    return new_dest;
}


char *strncpy(char* dest, const char* src, int n){
	char* new_dest = dest;
	while(n-- && *src){
	*dest = *src;
        dest++; src++;
	}	
        *dest ='\0';

    return new_dest;
}

int atoi(char *str)
{
        int pos = 0;
        char buffer[10] = {'\0'};
        strcpy(buffer,str);

        while ((buffer[pos] != '\0') && (buffer[pos] != '\n')){
        pos++;
        }
        pos--;

        int val = 0;
        int io = 1;
        while (pos >= 0) {
                buffer[pos] = buffer[pos] - 48;
                val = val + buffer[pos]*io;
                pos--;
                io= io*10;
        }

        return val;
}

void* memset(void *str,int val,uint64_t size)
{
    unsigned char* ptr = str;
    while(size--)
        *ptr++ = (unsigned char) val;
    return str;
}


char* strtok(char * str, char *delimiter)
{
    static int pos;
    static char *s;
    int start = pos;

    if(str!=NULL)
    s = str;

    int j = 0;

    while(s[pos] != '\0')
    {
        j = 0;
        while(delimiter[j] != '\0')
        {
            if(s[pos] == delimiter[j])
            {
                s[pos] = '\0';
                pos = pos+1;
                if(s[start] != '\0')
                return (&s[start]);
                else{
                start = pos; pos--;
                    break;
                }
            }
              j++;
        }
              pos++;
    }
    s[pos] = '\0';
    if(s[start] == '\0')
    return NULL;
      else
    return &s[start];
}


void *memcpy(void *dest, const void *src, uint64_t n)
{
    unsigned char *pd = (unsigned char *)dest;
    const unsigned char *ps = (unsigned char *)src;
    if ( ps < pd )
        for (pd += n, ps += n; n--;)
            *--pd = *--ps;
    else
        while(n--)
            *pd++ = *ps++;
    return dest;
}

int strcat(char *str1,char *str2){
	int i=0;int j=0;
	int len1 = 0,len2=0;
	len1 = strlen(str1);
	len2 = strlen(str2);
	for(i=len1;i<len1+len2;i++){
		str1[i]=str2[j];
		j++;
	}
	str1[i]='\0';
	return 0;
}
   

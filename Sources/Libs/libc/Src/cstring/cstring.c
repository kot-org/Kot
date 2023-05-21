#include <string.h>

// alpha size of int
int64_t asi(int64_t n, int basenumber){
    int64_t i = 0;
    do {
        i++;
    } while(n /= basenumber);
    return i;
}

int strlen(char* p){
    if(p == NULL) return 0;
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

int strncmp(char* a, char* b, size64_t len){
    for(int i = 0 ; i < len; i++){
        if(a[i] != b[i]){
            return false;
        }
    }
    return true;
}

void strcpy(char* to, char* from){
    while(*to++ = *from++);
    *to = 0;
}

char* strcat(char* dst, char* src){
    strcpy((char*)((uint64_t)dst + strlen(dst)), src);
    return dst;
}

char* strrchr(const char* s, int c){
    while(*s != '\0'){
        if(*s == c){
            return (char*)s;
        }
        s++;
    }
    if(*s == c){
        return (char*)s;
    }
    return NULL;
}

int strtol(const char* str, char** endptr, int8_t basenumber) {
    int result = 0;
    int8_t sign = 1;
    
    while (*str == ' ' || *str == '\t' || *str == '\n') str++;
    
    if(*str == '+')
        str++;
    else if(*str == '-') {
        sign = -1;
        str++;
    }

    while(*str) {
        if(*str >= '0' && *str <= '9')
            result = result * basenumber + (*str - '0');
        else if(*str >= 'a' && *str <= 'f')
            result = result * basenumber + (*str - 'a' + 10);
        else if(*str >= 'A' && *str <= 'F')
            result = result * basenumber + (*str - 'A' + 10);
        else
            break;
        str++;
    }

    if(endptr) 
        *endptr = (char*) str;

    return result * sign;
}

char** strsplit(char* str, const char* delimiters, uint64_t* count){
    char* entry = str;
    char* strTmp = str;
    uint64_t len = strlen(str);
    uint64_t currentItemNumber = 2; /* One entry for data and the other one for the NULL entry */
    uint64_t checkNum = 0;
    char* c = delimiters;
    uint64_t currentCharNumber = 0;
    uint64_t charNumberStart = 0;
    uint64_t lastCharEnd = 0;
    
    while(*strTmp != 0){  
        if(*strTmp == *c){
            checkNum++;
            c++;
        }else{
            c = delimiters;
            checkNum = 0;
        }
        
        if(checkNum >= strlen(delimiters)){
            currentItemNumber++;
            checkNum = 0;                         
        }
        strTmp++; 
    }

    char** ReturnValue = (char**)malloc(currentItemNumber * sizeof(char*));
    currentItemNumber = 0;
    checkNum = 0;
    while(*str != 0){
        currentCharNumber++;

        if(*str == *c){
            checkNum++;
            c++;
        }else{
            c = delimiters;
            checkNum = 0;
        }
        
        if(checkNum >= strlen(delimiters)){                   
            c = delimiters;

            if(currentCharNumber < strlen(delimiters)){
                charNumberStart = 0;
            }else{
                charNumberStart = currentCharNumber - strlen(delimiters);
            }

            ReturnValue[currentItemNumber] = (char*)malloc(((charNumberStart - lastCharEnd) + 1) * sizeof(char));
            memcpy((uintptr_t)ReturnValue[currentItemNumber], (uintptr_t)((uint64_t)entry + lastCharEnd), (charNumberStart - lastCharEnd) * sizeof(char));  
            ReturnValue[currentItemNumber][(charNumberStart - lastCharEnd)] = (char*)NULL;
            lastCharEnd = currentCharNumber;
            c = delimiters;
            
            currentItemNumber++;
            checkNum = 0;                         
        }
        str++; 
    }
    ReturnValue[currentItemNumber] = (char*)malloc(((len - lastCharEnd) + 1) * sizeof(char));
    memcpy((uintptr_t)ReturnValue[currentItemNumber], (uintptr_t)((uint64_t)entry + lastCharEnd), (len - lastCharEnd) * sizeof(char)); 
    ReturnValue[currentItemNumber][(len - lastCharEnd)] = (char*)NULL;
    ReturnValue[currentItemNumber + 1] = (char*)NULL;
    *count = currentItemNumber + 1;

    return ReturnValue;
}

void freeSplit(char** splitData){
    uint64_t i = 0;
    while(splitData[i] != NULL){
        free(splitData[i]);
        i++;
    }
    
    free(splitData);
}

int atoi(const char* str){
    int sign = 1, base = 0, i = 0;
     
    while (str[i] == ' ')
    {
        i++;
    }
     
    if (str[i] == '-' || str[i] == '+')
    {
        sign = 1 - 2 * (str[i++] == '-');
    }
   
    while (str[i] >= '0' && str[i] <= '9')
    {
        base = 10 * base + (str[i++] - '0');
    }
    return base * sign;
}
 
char* itoa(int64_t n, char* buffer, int basenumber){
	int64_t hold;
	int64_t i, j;
	hold = n;
	i = 0;

    bool IsNegative = (n < 0);

    if(IsNegative){
        n = -n;
    }
	
	do{
		hold = n % basenumber;
		buffer[i++] = (hold < 10) ? (hold + '0') : (hold + 'a' - 10);
	} while(n /= basenumber);

    if(IsNegative){
        buffer[i++] = '-';
    }

	buffer[i--] = NULL;
	
	for(j = 0; j < i; j++, i--)
	{
		hold = buffer[j];
		buffer[j] = buffer[i];
		buffer[i] = hold;
	}


    return buffer;
}
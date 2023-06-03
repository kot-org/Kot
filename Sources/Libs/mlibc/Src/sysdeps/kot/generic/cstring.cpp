#include <kot/cstring.h>

#include <string.h>
#include <stdlib.h>

extern "C" {

// alpha size of int
int64_t asi(int64_t n, int basenumber){
    int64_t i = 0;
    do {
        i++;
    } while(n /= basenumber);
    return i;
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


char** strsplit(char* str, char* delimiters, uint64_t* count){
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
            memcpy((void*)ReturnValue[currentItemNumber], (void*)((uint64_t)entry + lastCharEnd), (charNumberStart - lastCharEnd) * sizeof(char));  
            ReturnValue[currentItemNumber][(charNumberStart - lastCharEnd)] = '\0';
            lastCharEnd = currentCharNumber;
            c = delimiters;
            
            currentItemNumber++;
            checkNum = 0;                         
        }
        str++; 
    }
    ReturnValue[currentItemNumber] = (char*)malloc(((len - lastCharEnd) + 1) * sizeof(char));
    memcpy((void*)ReturnValue[currentItemNumber], (void*)((uint64_t)entry + lastCharEnd), (len - lastCharEnd) * sizeof(char)); 
    ReturnValue[currentItemNumber][(len - lastCharEnd)] = '\0';
    ReturnValue[currentItemNumber + 1] = (char*)NULL;
    *count = currentItemNumber + 1;

    return ReturnValue;
}


void freestrsplit(char** splitData){
    uint64_t i = 0;
    while(splitData[i] != NULL){
        free(splitData[i]);
        i++;
    }
    
    free(splitData);
}

}
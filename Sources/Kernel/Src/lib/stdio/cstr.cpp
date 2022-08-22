#include <lib/stdio/cstr.h>

char uintTo_StringOutput[128];
const char* to_string(uint64_t value){
    uint8_t size;
    uint64_t sizeTest = value;
    while (sizeTest / 10 > 0){
        sizeTest /= 10;
        size++;
    }

    uint8_t index = 0;
    while(value / 10 > 0){
        uint8_t remainder = value % 10;
        value /= 10;
        uintTo_StringOutput[size - index] = remainder + '0';
        index++;
    }
    uint8_t remainder = value % 10;
    uintTo_StringOutput[size - index] = remainder + '0';
    uintTo_StringOutput[size + 1] = 0; 
    return uintTo_StringOutput;
}

char hexTo_StringOutput[128];
const char* to_hstring(uint64_t value){
    uint64_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 8 * 2 - 1;
    for (uint8_t i = 0; i < size; i++){
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_StringOutput[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexTo_StringOutput[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    hexTo_StringOutput[size + 1] = 0;
    return hexTo_StringOutput;
}

char hexTo_StringOutput32[128];
const char* to_hstring(uint32_t value){
    uint32_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 4 * 2 - 1;
    for (uint8_t i = 0; i < size; i++){
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_StringOutput32[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexTo_StringOutput32[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    hexTo_StringOutput32[size + 1] = 0;
    return hexTo_StringOutput32;
}

char hexTo_StringOutput16[128];
const char* to_hstring(uint16_t value){
    uint16_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 2 * 2 - 1;
    for (uint8_t i = 0; i < size; i++){
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_StringOutput16[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexTo_StringOutput16[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    hexTo_StringOutput16[size + 1] = 0;
    return hexTo_StringOutput16;
}

char hexTo_StringOutput8[128];
const char* to_hstring(uint8_t value){
    uint8_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 1 * 2 - 1;
    for (uint8_t i = 0; i < size; i++){
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_StringOutput8[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexTo_StringOutput8[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    hexTo_StringOutput8[size + 1] = 0;
    return hexTo_StringOutput8;
}


char intTo_StringOutput[128];
const char* to_string(int64_t value){
    uint8_t isNegative = 0;

    if (value < 0){
        isNegative = 1;
        value *= -1;
        intTo_StringOutput[0] = '-';
    }

    uint8_t size;
    uint64_t sizeTest = value;
    while (sizeTest / 10 > 0){
        sizeTest /= 10;
        size++;
    }

    uint8_t index = 0;
    while(value / 10 > 0){
        uint8_t remainder = value % 10;
        value /= 10;
        intTo_StringOutput[isNegative + size - index] = remainder + '0';
        index++;
    }
    uint8_t remainder = value % 10;
    intTo_StringOutput[isNegative + size - index] = remainder + '0';
    intTo_StringOutput[isNegative + size + 1] = 0; 
    return intTo_StringOutput;
}

char doubleTo_StringOutput[128];
const char* to_string(double value, uint8_t decimalPlaces){
    if (decimalPlaces > 20) decimalPlaces = 20;

    char* intPtr = (char*)to_string((int64_t)value);
    char* doublePtr = doubleTo_StringOutput;

    if (value < 0){
        value *= -1;
    }

    while(*intPtr != 0){
        *doublePtr = *intPtr;
        intPtr++;
        doublePtr++;
    }

    *doublePtr = '.';
    doublePtr++;

    double newValue = value - (int)value;

    for (uint8_t i = 0; i < decimalPlaces; i++){
        newValue *= 10;
        *doublePtr = (int)newValue + '0';
        newValue -= (int)newValue;
        doublePtr++;
    }

    *doublePtr = 0;
    return doubleTo_StringOutput;
}

const char* to_string(double value){
    return to_string(value, 2);
}

const char* to_string(int value) {
    return to_string((uint64_t)value);
}

char chartostrOutput[2];
const char* to_string(char c){
    chartostrOutput[0] = c;
    chartostrOutput[1] = 0;
    return chartostrOutput;
}

int strlen(char*p){
    int count = 0;
    while (*p != '\0') {
        count++;
        p++;
    }
    return count;
}

bool strcmp(char* a, char* b){
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

bool strcmp(char* a, char* b, size64_t size){
    for(int i = 0 ; i < size; i++){
        if(a[i] != b[i]){
            return false;
        }
    }
    return true;
}


Node* split(char* str, char* delimiters){
    char* entry = str;
    char* entryCount = str;

    int len = strlen(str);
    char* c = delimiters;

    int currentItemNumber = 0;
    int currentCharNumber = 0;
    int ItemNumber = 1;
    int checkNum = 0;
    int charNumberStart = 0;

    int lastCharEnd = 0;
    
    while(*entryCount != 0){  
        if(*entryCount == *c){
            checkNum++;
            c++;
        }else{
            c = delimiters;
            checkNum = 0;
        }
        
        if(checkNum >= strlen(delimiters)){                               
            ItemNumber++;
            checkNum = 0;                         
        }
        entryCount++; 
    }
     
    Node* ReturnValue = CreateNode(0);

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
            
            charNumberStart = currentCharNumber - strlen(delimiters);

            if(charNumberStart < 0){
                charNumberStart = 0;
            }

            uint64_t StrSize = sizeof(char) * (charNumberStart - lastCharEnd);
            uintptr_t data = malloc(StrSize + 1);
            memcpy(data, (uintptr_t)&entry[lastCharEnd], StrSize);
            *(uint8_t*)((uint64_t)data + StrSize) = 0;
            ReturnValue->data = data;
            ReturnValue = ReturnValue->Add(0);

            lastCharEnd = charNumberStart + strlen(delimiters);
            c = delimiters;
            
            currentItemNumber++;
            checkNum = 0;                         
        }
        str++; 
    }

    uintptr_t data = malloc(sizeof(char) * (len - lastCharEnd));
    memcpy(data, (uintptr_t)&entry[lastCharEnd], len - lastCharEnd);
    ReturnValue->data = data;
    ReturnValue->next = NULL;
    return ReturnValue->parent;
}

char returnChar[128];
char* ConvertByte(uint64_t bytes){
    char* units[5];
    units[0] = " B";
    units[1] = " KB";
    units[2] = " MB";
    units[3] = " GB";
    units[4] = " TB";

    int i;
    while(bytes / 1024 > 1){
        bytes = (double)(bytes / 1024);
        i++;
        if(i > 3) break;
    }

    Message("RAM : %x", bytes);
    char* value = (char*)to_string(bytes);

    memcpy(returnChar, value, strlen(value));
    memcpy(returnChar + strlen(value), units[i], strlen(units[i]));
    returnChar[127] = 0;
    return returnChar;

}



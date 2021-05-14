#include "cstring.h"


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waggressive-loop-optimizations"

int isalpha(char _c)
{
    return  _c >= 'A' &&  _c <= 'Z' ||  _c >= 'a' &&  _c <= 'z'; 
}

char str[1025]; 
char* shorttostr(unsigned short* sh) {
    memset(str,0,1025);
	int strIndex = 0;
	for(int i = 0;i<1024;i++) {
		str[strIndex] = sh[i];
		strIndex++;
	}
	str[1024] = 0;
    return str;
}

char uintTo_StringOutput[128];
char* inttostr(uint64_t val) {
    memset(uintTo_StringOutput,0,128);
    uint8_t size;
    uint64_t sizeTest = val;
    while (sizeTest / 10 > 0){
        sizeTest /= 10;
        size++;
    }

    uint8_t index = 0;
    while(val / 10 > 0){
        uint8_t remainder = val % 10;
        val /= 10;
        uintTo_StringOutput[size - index] = remainder + '0';
        index++;
    }
    uint8_t remainder = val % 10;
    uintTo_StringOutput[size - index] = remainder + '0';
    uintTo_StringOutput[size + 1] = 0; 
    return uintTo_StringOutput;
}

char* inttostr(uint32_t val) {
    return inttostr((uint64_t)val);
}
char* inttostr(uint16_t val) {
    return inttostr((uint64_t)val);
}
char* inttostr(uint8_t val) {
    return inttostr((uint64_t)val);
}

char* inttostr(int val) {
    return inttostr((uint64_t)val);
}

char intTo_StringOutput[128];
char* inttostr(int64_t val) {
    memset(intTo_StringOutput,0,128);
    uint8_t isNegative = 0;
    if(val < 0) {
        isNegative = 1;
        val *= -1;
        intTo_StringOutput[0] = '-';
    }

    uint8_t size;
    uint64_t sizeTest = val;
    while (sizeTest / 10 > 0){
        sizeTest /= 10;
        size++;
    }

    uint8_t index = 0;
    while(val / 10 > 0){
        uint8_t remainder = val % 10;
        val /= 10;
        intTo_StringOutput[isNegative + size - index] = remainder + '0';
        index++;
    }
    uint8_t remainder = val % 10;
    intTo_StringOutput[isNegative + size - index] = remainder + '0';
    intTo_StringOutput[isNegative + size + 1] = 0; 
    return intTo_StringOutput;
}

char doubleTo_StringOutput[128];
char* inttostr(double val,uint8_t decimalPlaces) {
    memset(doubleTo_StringOutput,0,128);
    if(decimalPlaces > 10) decimalPlaces = 10;
    char* intPtr = (char*)inttostr((int64_t)val);
    char* doublePtr = doubleTo_StringOutput;

    if (val < 0) {
        val *=-1;
    }

    while(*intPtr != 0) {
        *doublePtr = *intPtr;
        intPtr++;
        doublePtr++;
    }

    *doublePtr = '.';
    doublePtr++;
    double newVal = val - (int)val;

    for(uint8_t i = 0;i<decimalPlaces;i++) {
        newVal *= 10;
        *doublePtr  = (int)newVal + '0';
        newVal -= (int)newVal;
        doublePtr++;
    }

    *doublePtr = 0;
    return doubleTo_StringOutput;
}

char* inttostr(double val) {
    return inttostr(val,2);
}


char hexTo_StringOutput[128];
char* inttohstr(uint64_t val) {
    memset(hexTo_StringOutput,0,128);
    uint64_t* valPtr = &val;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 8*2-1;
    for(uint8_t i = 0;i< size;i++) {
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_StringOutput[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexTo_StringOutput[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }

    hexTo_StringOutput[size+1] = 0;
    return hexTo_StringOutput;
}

char hexTo_StringOutput32[128];
char* inttohstr(uint32_t val) {
    memset(hexTo_StringOutput32,0,128);
    uint32_t* valPtr = &val;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 4*2-1;
    for(uint8_t i = 0;i< size;i++) {
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_StringOutput32[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexTo_StringOutput32[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }

    hexTo_StringOutput32[size+1] = 0;
    return hexTo_StringOutput32;
}

char hexTo_StringOutput16[128];
char* inttohstr(uint16_t val) {
    memset(hexTo_StringOutput16,0,128);
    uint16_t* valPtr = &val;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 2*2-1;
    for(uint8_t i = 0;i< size;i++) {
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_StringOutput16[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexTo_StringOutput16[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }

    hexTo_StringOutput16[size+1] = 0;
    return hexTo_StringOutput16;
}

char hexTo_StringOutput8[128];
char* inttohstr(uint8_t val) {
    memset(hexTo_StringOutput8,0,128);
    uint8_t* valPtr = &val;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 1*2-1;
    for(uint8_t i = 0;i< size;i++) {
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_StringOutput8[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexTo_StringOutput8[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }

    hexTo_StringOutput8[size+1] = 0;
    return hexTo_StringOutput8;
}

char chartostrOutput[2];
char* chartostr(char c) {
    chartostrOutput[0] = c;
    chartostrOutput[1] = 0;
    return chartostrOutput;
}

int strlenOutput = 0;
int strlen(char* str) {
    for(int i = 0;str[i] != 0;i++)
        strlenOutput = i;
    return strlenOutput;
}

#pragma GCC diagnostic pop
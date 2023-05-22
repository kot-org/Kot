#include <assert.h>

void _assert(int expression, const char* expression_name, const char* file, int line, const char* function){
    if(!expression){
        char str[512];
        char intbuf[33];
        strcat(str, "assert: ");
        strcat(str, file);
        strcat(str, ":");
        strcat(str, itoa(line, &intbuf, 10));
        strcat(str, ": ");
        strcat(str, function);
        strcat(str, ": Assertion: '");
        strcat(str, expression_name);
        strcat(str, "' failed");
        strcat(str, "\0");
        kot_Printlog(str);
        Sys_Close(KFAIL);
    }
}
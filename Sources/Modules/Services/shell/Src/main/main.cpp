#include <main/main.h>

char* TableConverter;
size64_t TableConverterCharCount;

extern "C" int main(){
    GetTableConverter("d0:azerty.bin", &TableConverter, &TableConverterCharCount);

    SrvInitalize();

    return KSUCCESS;
}
#include <core/main.h>

void* TableConverter;
size64_t TableConverterCharCount;

int main(){
    kot_GetTableConverter("d0:azerty.bin", &TableConverter, &TableConverterCharCount);

    SrvInitalize();

    return KSUCCESS;
}
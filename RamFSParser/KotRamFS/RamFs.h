#pragma once
#include <iostream>
#pragma warning(disable : 4996)

#define arg_outfile 1
#define arg_infiles 2
#define VERSION 1
#define MaxName 36

#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))

PACK(struct Header {
    char signature[5];
    uint64_t version;
    uint64_t filenumber;
    uint64_t initfile;
});

PACK(struct File {
    char name[MaxName];
    uint64_t size;
});
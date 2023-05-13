#ifndef _AUTHORIZATION_H
#define _AUTHORIZATION_H 1

#include <kot/types.h>

namespace Kot{
    #define AUTHORIZATION_FS                0x0

    #define FS_AUTHORIZATION_BASIC          0x0
    #define FS_AUTHORIZATION_MEDIUM         0x1
    #define FS_AUTHORIZATION_HIGH           0x2

    typedef uint64_t authorization_t;
    typedef uint64_t authorization_type_t;

    typedef struct{
        char* Value;
        uint64_t Radio;
        bool IsValidate;
    }validation_field_t;

    typedef struct{
        uint64_t PID;
        char* Title;
        char* Message;
        uint64_t ValidationFieldsCount;
        validation_field_t* ValidationFields;
    }autorization_field_t;

    KResult kot_GetAuthorization(autorization_field_t* data, bool IsAwait);
}

#endif
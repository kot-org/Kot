#ifndef KOT_AUTHORIZATION_H
#define KOT_AUTHORIZATION_H 1

#include <kot/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define AUTHORIZATION_FS                0x0

#define FS_AUTHORIZATION_BASIC          0x0
#define FS_AUTHORIZATION_MEDIUM         0x1
#define FS_AUTHORIZATION_HIGH           0x2

typedef uint64_t kot_authorization_t;
typedef uint64_t kot_authorization_type_t;

typedef struct{
    char* Value;
    uint64_t Radio;
    bool IsValidate;
}kot_validation_field_t;

typedef struct{
    uint64_t PID;
    char* Title;
    char* Message;
    uint64_t ValidationFieldsCount;
    kot_validation_field_t* ValidationFields;
}kot_autorization_field_t;

KResult kot_GetAuthorization(kot_autorization_field_t* data, bool IsAwait);

#if defined(__cplusplus)
} 
#endif

#endif
#include <kot/authorization.h>
#include <stdlib.h>
#include <string.h>

KResult kot_GetAuthorization(kot_autorization_field_t* data, bool IsAwait){
    // TODO
    data->ValidationFields[0].IsValidate = true;
    return KSUCCESS;
}

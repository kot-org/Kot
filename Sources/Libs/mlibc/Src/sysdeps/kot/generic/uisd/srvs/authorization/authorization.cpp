#include <kot/authorization.h>
#include <stdlib.h>
#include <string.h>

namespace Kot{
    KResult GetAuthorization(autorization_field_t* data, bool IsAwait){
        // TODO
        data->ValidationFields[0].IsValidate = true;
        return KSUCCESS;
    }
}

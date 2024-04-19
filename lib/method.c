#include <string.h>


#ifndef METHOD_HEADER_DEFINE__
#define METHOD_HEADER_DEFINE__
#include "../includes/method.h"
#endif // METHOD_HEADER_DEFINE__


enum Method get_req_method(char *method)
{

    if (!strcmp(method, _GET))
    {
        return GET;
    }
    if (!strcmp(method, _POST))
    {
        return POST;
    }
    return INVALID_METHOD;
}
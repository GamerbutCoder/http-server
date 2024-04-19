#include <stddef.h>


#ifndef REQUEST_HEADER_DEFINE__
#define  REQUEST_HEADER_DEFINE__
#include "request.h"
#endif // REQUEST_HEADER_DEFINE__

#ifndef METHOD_HEADER_DEFINE__
#define METHOD_HEADER_DEFINE__
#include "method.h"
#endif // METHOD_HEADER_DEFINE__


typedef struct Response
{
    enum Method method;
    int status_code;
    char *message;
    char *content_type;
    char *path;
    char *protocol;
    char *body;
    size_t content_length;
    long int file_size;
} Response;

Response handle_response(Request *req);

char* arrange_response(Response *res);

void freeResponse(Response *res);
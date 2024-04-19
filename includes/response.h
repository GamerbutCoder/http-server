#include <stddef.h>

#ifndef REQUEST_HEADER_DEFINE__
#define REQUEST_HEADER_DEFINE__
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
    char *file_format;

} Response;

Response handle_response(Request *req);

void write_in_html_format(Response *res, char *body);

void freeResponse(Response *res);
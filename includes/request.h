#include <stddef.h>

#ifndef METHOD_HEADER_DEFINE__
#define METHOD_HEADER_DEFINE__
#include "method.h"
#endif // METHOD_HEADER_DEFINE__

enum bool {
    FALSE,
    TRUE
};

typedef struct Request
{
    enum Method method;
    char *path;
    char *protocol;
    size_t content_length;
    enum bool is_valid;
    
} Request;

Request parse_http_req(char *req, size_t req_size);

void freeRequest(Request *req);

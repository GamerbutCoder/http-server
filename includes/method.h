#define _GET "GET"
#define _POST "POST"

enum Method
{
    GET,
    POST,
    PUT,
    OPTIONS,
    DELETE,
    INVALID_METHOD
};

enum Method get_req_method(char *req);
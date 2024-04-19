
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef REQUEST_HEADER_DEFINE__
#define REQUEST_HEADER_DEFINE__
#include "../includes/request.h"
#endif // REQUEST_HEADER_DEFINE__

void freeRequest(Request *req)
{
    if (req->path != NULL)
    {
        free(req->path);
        req->path = NULL;
    }
    if (req->protocol != NULL)
    {
        free(req->protocol);
        req->protocol = NULL;
    }
    req->content_length = 0;
}

Request parse_first_line(char *line, size_t len)
{

    Request req;

    int i = 0, index = 0, nth_space = 0;

    char word[len + 1];

    for (; i < len + 1; i++)
    {
        if (line[i] == ' ' || line[i] == '\0')
        {
            word[index] = '\0';
            switch (nth_space)
            {
            case 0:
                req.method = get_req_method(word);
                break;
            case 1:
                req.path = (char *)malloc(sizeof(char) * (index + 1));
                strncpy(req.path, word, index + 1);
                // req.path[index] = '\0';
                break;
            case 2:
                req.protocol = (char *)malloc(sizeof(char) * (index + 1));
                strncpy(req.protocol, word, index + 1);
                // req.protocol[index] = '\0';
                break;
            default:
                break;
            }
            nth_space++;
            index = 0;
        }
        else
        {
            word[index] = line[i];
            index++;
        }
    }

    printf("parsed req\n\tEnum method: %d\n\tPath: %s :: %ld\n\tProtocol: %s :: %ld\n", req.method, req.path, strlen(req.path), req.protocol, strlen(req.protocol));
    return req;
}


Request parse_http_req(char *req, size_t req_size)
{

    int size = 0;

    while ((size < req_size) && (*(req + size) != '\r'))
    {
        size++;
    }

    // printf("size: %d\n", size);
    
    char *line = (char *)malloc(sizeof(char) * (size + 1));

    strncpy(line, req, size);

    line[size] = '\0';

    printf("First line: %s\n", line);

    Request request = parse_first_line(line, size);

    // printf("Method: %d\nPath: %s\nProtocol: %s\n %p\n", request->method, request->path, request->protocol, request);

    return request;
}
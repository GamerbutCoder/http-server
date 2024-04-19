
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

int valid_file_format(char *path)
{
    // printf("verifying file format in request.c : %s\n", path);
    if (path == NULL)
    {
        return -1;
    }
    // printf("valid_file_format: %s\n", path);
    int n = strlen(path);
    // html
    if (n >= 6 && (path[n - 5] == '.') && (path[n - 4] == 'h') && (path[n - 3] == 't') && (path[n - 2] == 'm') && (path[n - 1] == 'l'))
    {
        return 0;
    }
    // jpeg
    else if (n >= 6 && (path[n - 5] == '.') && (path[n - 4] == 'j') && (path[n - 3] == 'p') && (path[n - 2] == 'e') && (path[n - 1] == 'g'))
    {
        return 0;
    }
    // ico
    else if (n >= 5 && ((path[n - 4] == '.') && (path[n - 3] == 'i') && (path[n - 2] == 'c') && (path[n - 1] == 'o')))
    {
        return 0;
    }
    // png
    else if (n >= 5 && ((path[n - 4] == '.') && (path[n - 3] == 'p') && (path[n - 2] == 'n') && (path[n - 1] == 'g')))
    {
        return 0;
    }
    // css
    else if (n >= 5 && ((path[n - 4] == '.') && (path[n - 3] == 'c') && (path[n - 2] == 's') && (path[n - 1] == 's')))
    {
        return 0;
    }
    // gif
    else if (n >= 5 && ((path[n - 4] == '.') && (path[n - 3] == 'g') && (path[n - 2] == 'i') && (path[n - 1] == 'f')))
    {
        return 0;
    }
    // js
    else if (n >= 4 && ((path[n - 3] == '.') && (path[n - 2] == 'j') && (path[n - 1] == 's')))
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

Request parse_first_line(char *line, size_t len)
{

    Request req;
    req.path = NULL;
    req.protocol = NULL;
    req.content_length = 0;
    req.is_valid = TRUE;

    int i = 0, index = 0, nth_space = 0;

    char word[len + 1];

    for (; ((i < (len + 1)) && req.is_valid); i++)
    {
        // printf("line %d %c\n", line[i], line[i]);
        if (line[i] == ' ' || line[i] == '\0')
        {
            word[index] = '\0';
            switch (nth_space)
            {
            case 0:
                req.method = get_req_method(word);

                if (req.method == INVALID_METHOD)
                {
                    req.is_valid = FALSE;
                }

                break;
            case 1:
                req.path = (char *)malloc(sizeof(char) * (index + 1));
                strncpy(req.path, word, index);
                req.path[index] = '\0';
                break;
            case 2:
                req.protocol = (char *)malloc(sizeof(char) * (index + 1));
                strncpy(req.protocol, word, index);
                req.protocol[index] = '\0';
                req.is_valid = (req.is_valid && 1);
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

    if (valid_file_format(req.path) < 0)
    {
        req.is_valid = FALSE;
    }

    printf("Request \n\tEnum method: %d\n\tPath: %s :: %ld\n\tProtocol: %s :: %ld\n\tValid :: %d\n", req.method, req.path, strlen(req.path), req.protocol, strlen(req.protocol), req.is_valid);
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

    //  printf("line is null terminated %d %d %d %s\n", line[size-2], line[size-1], line[size], line);

    // printf("First line: %s\n", line);

    Request request = parse_first_line(line, size);

    free(line);

    line = NULL;

    // printf("Method: %d\nPath: %s\nProtocol: %s\n %p\n", request->method, request->path, request->protocol, request);

    return request;
}
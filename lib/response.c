#include <sys/stat.h>
#include <sys/types.h>
#include <bits/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef RESPONSE_HEADER_DEFINE__
#define RESPONSE_HEADER_DEFINE__
#include "../includes/response.h"
#endif

#ifndef HEADER_HEADER_DEFINE__
#define HEADER_HEADER_DEFINE__
#include "../includes/headers.h"
#endif

void freeResponse(Response *res)
{
    if (res->message != NULL)
    {
        free(res->message);
        res->message = NULL;
    }
    if (res->content_type != NULL)
    {
        free(res->content_type);
        res->content_type = NULL;
    }
    if (res->path != NULL)
    {
        free(res->path);
        res->path = NULL;
    }
    if (res->protocol != NULL)
    {
        free(res->protocol);
        res->protocol = NULL;
    }
    if (res->body != NULL)
    {
        free(res->body);
        res->body = NULL;
    }

    // if(res->content_length != NULL){
    //     free(res->content_length);
    //     res->content_length = NULL;
    // }
}

int valid_file_format(char *path)
{
    // printf("valid_file_format: %s\n", path);
    int n = strlen(path);
    if (n >= 6 && (path[n - 5] == '.') && (path[n - 4] == 'h') && (path[n - 3] == 't' && ((path[n - 2] == 'm') && (path[n - 1] == 'l'))))
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

/*
if path exists return its file size
*/
long int find_file_size(char *path)
{
    struct stat buf;
    if (stat(path, &buf) == 0)
    {
        // printf("stat succeeded %ld\n", buf.st_size);
        return buf.st_size;
    }
    else
    {
        // printf("stat failed %s\n", path);
        return -1;
    }
}

void write_file_contents_to_body(char *body, char *path)
{

    FILE *f = fopen(path, "r");
    if (f != NULL)
    {
        size_t n;
        char *b = (char *)malloc(101);
        while ((n = fread(b, 1, 100, f)) > 0)
        {
            strncat(body, b, n);
        }
        fclose(f);
    }
}

char *arrange_response(Response *res)
{

    char *body = (char *)malloc(sizeof(char) * (res->file_size + (1024 * 1))); // file size + 1000 bytes extra space for writing headers and stuff

    printf("body address: %p file size: %ld\n", body, res->file_size);

    // body[0] = '\0';
    strncat(body, res->protocol, (strlen(res->protocol) + 1));

    switch (res->status_code)
    {
    case 200:
        strncat(body, " 200 ", 6);
        break;
    case 400:
        strncat(body, " 400 ", 6);
        break;
    case 404:
        strncat(body, " 404 ", 6);
        break;
    default:
        break;
    }

    strncat(body, res->message, (strlen(res->message) + 1));

    strncat(body, "\r\n\0", 3);

    if (res->content_type != NULL)
    {
        strncat(body, _CONTENT_TYPE, (strlen(_CONTENT_TYPE) + 1));

        strncat(body, res->content_type, (strlen(res->content_type) + 1));

        strncat(body, "\r\n\0", 3);
    }

    //// printf("line263 %s\n", body);

    if (res->file_size > 0)
    {
        strncat(body, _CONTENT_LENGTH, (strlen(_CONTENT_LENGTH) + 1));

        char *file_size = (char *)malloc(sizeof(char) * 10);

        sprintf(file_size, "%ld", res->file_size);

        strncat(body, file_size, (strlen(file_size) + 1));

        free(file_size);

        strncat(body, "\r\n\0", 3);
    }

    strncat(body, "\r\n\0", 3);

    if (res->file_size > 0)
    {
        write_file_contents_to_body(body, res->path);
        //// printf("line274 %s\n", body);
        strncat(body, "\r\n\0", 3);
    }

    printf("response size:- %ld\n", strlen(body));

    char *final_response_body = (char *)malloc(sizeof(char) * (strlen(body) + 1));

    printf("final body: %p\n", final_response_body);

    strcpy(final_response_body, body);

    printf("-------------------------response -------------------------------\n%s", final_response_body);
    // // strncat
    free(body);

    // return body;
    return final_response_body;
}

Response handle_response(Request *req)
{

    Response res;

    res.content_length = 0;
    res.file_size = 0;
    size_t req_protocol_len = strlen(req->protocol);
    res.protocol = (char *)malloc(sizeof(char) * (req_protocol_len + 1));
    res.content_type = (char *)malloc(sizeof(char) * (strlen(_TEXT_HTML) + 1));
    strcpy(res.content_type, _TEXT_HTML);
    char *path = (char *)malloc(sizeof(char) * (3 + strlen(req->path)));
    res.body = NULL;

    // printf("Res address: %p\n", res);

    //// printf("bfr handle_req %d %s\n", req->method, req->path);
    res.method = req->method;
    strcpy(res.protocol, req->protocol);

    path[0] = '.';
    path[1] = '\0';
    // strncat(path, ".", 2);
    strncat(path, req->path, (strlen(req->path) + 1));
    // strncat(path, "\0", 2);
    printf("checking path: %s\n", path);
    res.path = path;

    path = NULL;

    if (valid_file_format(req->path) < 0)
    {
        res.status_code = 400;
        res.message = (char *)malloc(strlen((_BAD_REQUEST) + 1));
        strcpy(res.message, _BAD_REQUEST);
        return res;
    }

    long int file_size;

    //// printf("aft2 handle_req %s %ld\n", res->path, res->method);

    file_size = find_file_size(path);

    // printf("checking if this path exists: %s %ld\n", path, file_size);

    if (file_size < 0)
    {
        res.status_code = 404;
        res.message = (char *)malloc(strlen((_NOT_FOUND) + 1));
        strcpy(res.message, _NOT_FOUND);
        // printf("path not found %s\n", res->message);
        return res;
    }
    else
    {
        // printf("path found with file size:  %ld\n", file_size);
        // res->path = path;
        // (char *)malloc((strlen(path)+1));
        // strcpy(res->path, path);
    }

    // free(path);

    res.status_code = 200;

    res.message = (char *)malloc(strlen((_OK) + 1));

    strcpy(res.message, _OK);

    res.file_size = file_size;

    // res.body  = (char *)malloc(sizeof(char) * (res.file_size + (1024 * 10))); // // file size + 1000 bytes extra space for writing headers and stuff

    // arrange_response(&res, res.body);

    // printf("Response status: %d  message: %s \n", res->status_code, res->message);

    return res;
}

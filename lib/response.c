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

// MARK: freeResponse
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
    if (res->file_format != NULL)
    {
        free(res->file_format);
        res->file_format = NULL;
    }

    // if(res->content_length != NULL){
    //     free(res->content_length);
    //     res->content_length = NULL;
    // }
}
// MARK: getContentType
char *get_content_type(char *format)
{
    char *content_type = (char *)calloc(20, sizeof(char));

    if (!strcmp(format, _HTML))
    {
        strcpy(content_type, _TEXT_HTML);
    }
    else if (!strcmp(format, _CSS))
    {
        strcpy(content_type, _TEXT_CSS);
    }
    else if (!strcmp(format, _JS))
    {
        strcpy(content_type, _TEXT_JS);
    }
    else if (!strcmp(format, _PNG))
    {
        strcpy(content_type, _IMAGE_PNG);
    }
    else if (!strcmp(format, _JPEG))
    {
        strcpy(content_type, _IMAGE_JPEG);
    }
    else if (!strcmp(format, _GIF))
    {
        strcpy(content_type, _IMAGE_GIF);
    }
    else if (!strcmp(format, _ICO))
    {
        strcpy(content_type, _IMAGE_X_ICON);
    }

    return content_type;
}

// MARK: getFileFormat
char *get_file_format(char *path)
{
    char *format = (char *)calloc(5, sizeof(char));
    int n = strlen(path);
    if (n >= 6 && (path[n - 5] == '.') && (path[n - 4] == 'h') && (path[n - 3] == 't') && (path[n - 2] == 'm') && (path[n - 1] == 'l'))
    {
        strcpy(format, _HTML);
    }
    else if (n >= 6 && (path[n - 5] == '.') && (path[n - 4] == 'j') && (path[n - 3] == 'p') && (path[n - 2] == 'e') && (path[n - 1] == 'g'))
    {
        strcpy(format, _JPEG);
    }
    else if (n >= 5 && ((path[n - 4] == '.') && (path[n - 3] == 'i') && (path[n - 2] == 'c') && (path[n - 1] == 'o')))
    {
        strcpy(format, _ICO);
    }
    else if (n >= 5 && ((path[n - 4] == '.') && (path[n - 3] == 'p') && (path[n - 2] == 'n') && (path[n - 1] == 'g')))
    {
        strcpy(format, _PNG);
    }
    else if (n >= 5 && ((path[n - 4] == '.') && (path[n - 3] == 'c') && (path[n - 2] == 's') && (path[n - 1] == 's')))
    {
        strcpy(format, _CSS);
    }
    else if (n >= 5 && ((path[n - 4] == '.') && (path[n - 3] == 'g') && (path[n - 2] == 'i') && (path[n - 1] == 'f')))
    {
        strcpy(format, _GIF);
    }
    else if (n >= 4 && ((path[n - 3] == '.') && (path[n - 2] == 'j') && (path[n - 1] == 's')))
    {
        strcpy(format, _JS);
    }

    return format;
}

// MARK: findFileSize
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
// MARK: writeFileToBody
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
        free(b);
        fclose(f);
    }
}

// MARK: arrangeResponse
void write_in_html_format(Response *res, char *body)
{

    // printf("body address: %p file size: %ld\n", body, res->file_size);

    // body[0] = '\0';
    strcat(body, res->protocol);

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

    strcat(body, res->message);

    strcat(body, "\r\n");

    if (res->content_type != NULL)
    {
        strcat(body, _CONTENT_TYPE);

        strcat(body, res->content_type);

        strncat(body, "\r\n\0", 3);
    }

    //// printf("line263 %s\n", body);

    if (res->file_size > 0)
    {
        strcat(body, _CONTENT_LENGTH);

        char *file_size = (char *)malloc(sizeof(char) * 10);

        sprintf(file_size, "%ld", res->file_size);

        strcat(body, file_size);

        free(file_size);

        strcat(body, "\r\n");
    }

    strcat(body, "\r\n");

    if (res->file_size > 0)
    {

        //// printf("line274 %s\n", body);
        strcat(body, res->body);
        strcat(body, "\r\n");
    }

    printf("final response body len:- %ld\n================================\n%s\n================================\n", strlen(body), body);
}

// MARK: printResponse
void print_response(Response *res)
{
    printf("Response\n\tMethod :: %d\n\tStatus Code :: %d\n\tMessage :: %s\n\tProtocol :: %s\n\tPath :: %s\n\tFile Size/Content Length :: %ld\n\tFile Format :: %s\n\tContent Type :: %s\n\tBody :: \n-------------------\n%s\n-------------------\n",
           res->method, res->status_code, res->message, res->protocol, res->path, res->file_size, res->file_format, res->content_type, res->body);
}

// MARK: handleResponse
Response handle_response(Request *req)
{
    // printf("called handle reponse\n");
    Response res;

    res.content_length = 0;
    res.body = NULL;
    res.content_type = NULL;
    res.file_size = 0;
    res.message = NULL;
    res.path = NULL;
    res.protocol = NULL;
    res.status_code = 0;
    res.file_format = NULL;

    res.method = req->method;

    res.protocol = (char *)calloc(9, sizeof(char)); // HTTP/X.X\0 = 9 chars
    strcpy(res.protocol, req->protocol);

    if (req->is_valid)
    {
        res.status_code = 200;
        res.message = (char *)calloc((strlen(_OK) + 1), sizeof(char));
        strcpy(res.message, _OK);
    }
    else
    {
        res.message = (char *)calloc((strlen(_BAD_REQUEST) + 1), sizeof(char));
        strcpy(res.message, _BAD_REQUEST);
        res.status_code = 400;
        print_response(&res);
        return res;
    }

    res.path = (char *)calloc((strlen(req->path) + 2), sizeof(char));
    res.path[0] = '.';
    res.path[1] = '\0';
    strcat(res.path, req->path);

    res.file_size = find_file_size(res.path);
    // printf("res file path: %s\nfile_size: %ld\n", res.path, res.file_size);
    if (res.file_size == -1)
    {
        res.message = (char *)calloc((strlen(_NOT_FOUND) + 1), sizeof(char));
        strcpy(res.message, _NOT_FOUND);
        res.status_code = 404;
        print_response(&res);
        return res;
    }

    res.file_format = get_file_format(res.path);

    res.content_type = get_content_type(res.file_format);

    res.content_length = res.file_size;

    res.body = (char *)calloc((res.content_length + 10), sizeof(char));

    write_file_contents_to_body(res.body, res.path);

    // printf("body ends with null %d %d %d\n", res.body[res.content_length-2], res.body[res.content_length-1], res.body[res.content_length]);

    print_response(&res);

    return res;
}

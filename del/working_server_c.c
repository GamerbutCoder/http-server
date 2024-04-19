#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <unistd.h>
#include <asm-generic/socket.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <bits/types.h>

#define DEFAULT_PORT 8080
#define MAX_REQ_QUEUE_SIZE 10
#define MAX_REQ_SIZE 1024
#define DEFAULT_PROTOCOL "http"
#define _GET "GET"
#define _POST "POST"
#define _CONTENT_TYPE "Content-Type: "
#define _CONTENT_LENGTH "Content-Length: "
#define _BAD_REQUEST "Bad request"
#define _OK "OK"
#define _NOT_FOUND "Not found"
#define _TEXT_HTML "text/html"

enum Method
{
    GET,
    POST,
    PUT,
    OPTIONS,
    DELETE,
    INVALID_METHOD
};

typedef struct Request
{
    enum Method method;
    char *path;
    char *protocol;
    size_t content_length;
} Request;

typedef struct Respone
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
    free(req);
    req = NULL;
}

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

    if(res->content_length != NULL){
        free(res->content_length);
        res->content_length = NULL;
    }
    free(res);
    res = NULL;
}

enum Method get_req_method(char *req)
{

    if (!strcmp(req, _GET))
    {
        return GET;
    }
    if (!strcmp(req, _POST))
    {
        return POST;
    }
    return INVALID_METHOD;
}

Request *parse_first_line(char *line, size_t len)
{

    Request *req = (Request *)malloc(sizeof(Request));

    printf("Req address: %p\n", req);

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
                req->method = get_req_method(word);
                break;
            case 1:
                req->path = (char *)malloc(sizeof(char) * (index + 1));
                strncpy(req->path, word, index + 1);
                req->path[index] = '\0';
                break;
            case 2:
                req->protocol = (char *)malloc(sizeof(char) * (index + 1));
                strncpy(req->protocol, word, index + 1);
                req->protocol[index] = '\0';
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

    return req;
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

Request *parse_http_req(char *req, size_t req_size)
{

    int size = 0;

    while ((size < req_size) && (*(req + size) != '\r'))
    {
        size++;
    }

    // printf("size: %d\n", size);

    char *line = (char *)malloc(sizeof(char) * (size + 1));

    line = strncpy(line, req, size + 1);

    line[size] = '\0';

    printf("First line: %s\n", line);

    Request *request = parse_first_line(line, size);

    // printf("Method: %d\nPath: %s\nProtocol: %s\n %p\n", request->method, request->path, request->protocol, request);

    return request;
}

Response *handle_request(Request *req)
{

    Response *res = (Response *)malloc(sizeof(Response));

    printf("Res address: %p\n", res);

    //// printf("bfr handle_req %d %s\n", req->method, req->path);
    res->method = req->method;

    res->protocol = (char *)malloc(sizeof(char) * (strlen(req->protocol) + 1));

    strncpy(res->protocol, req->protocol, (strlen(req->protocol) + 1));

    res->protocol[strlen(req->protocol)] = '\0';

    // printf("handle_req res->protocol: %s\n", res->protocol);

    char *path = (char *)malloc(sizeof(char) * (3 + strlen(req->path)));
    strncat(path, ".", 2);
    strncat(path, req->path, (strlen(req->path) + 1));
    strncat(path, "\0", 2);

    res->path = path;

    if (valid_file_format(req->path) < 0)
    {
        res->status_code = 400;
        res->message = (char *)malloc(strlen((_BAD_REQUEST) + 1));
        strcpy(res->message, _BAD_REQUEST);
        return res;
    }

    long int file_size;

    //// printf("aft2 handle_req %s %ld\n", res->path, res->method);

    file_size = find_file_size(path);

    // printf("checking if this path exists: %s %ld\n", path, file_size);

    if (file_size < 0)
    {
        res->status_code = 404;
        res->message = (char *)malloc(strlen((_NOT_FOUND) + 1));
        strcpy(res->message, _NOT_FOUND);
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

    res->status_code = 200;
    res->message = (char *)malloc(strlen((_OK) + 1));
    strcpy(res->message, _OK);

    res->content_type = (char *)malloc(strlen((_TEXT_HTML) + 1));
    strcpy(res->content_type, _TEXT_HTML);
    res->file_size = file_size;

    // printf("Response status: %d  message: %s \n", res->status_code, res->message);

    return res;
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
            strncat(body, b, n + 1);
        }
        fclose(f);
    }
}

char *arrange_response(Response *res)
{

    char *body = (char *)malloc(sizeof(char) * (res->file_size + (1024 * 10))); // file size + 1000 bytes extra space for writing headers and stuff

    printf("body address: %p\n", body);

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

    strncat(body, "\r\n", 3);

    if (res->content_type != NULL)
    {
        strncat(body, _CONTENT_TYPE, (strlen(_CONTENT_TYPE) + 1));

        strncat(body, res->content_type, (strlen(res->content_type) + 1));

        strncat(body, "\r\n", 3);
    }

    //// printf("line263 %s\n", body);

    if (res->file_size > 0)
    {
        strncat(body, _CONTENT_LENGTH, (strlen(_CONTENT_LENGTH) + 1));

        char *file_size = (char *)malloc(sizeof(char) * 10);

        sprintf(file_size, "%ld", res->file_size);

        strncat(body, file_size, (strlen(file_size) + 1));

        free(file_size);

        strncat(body, "\r\n", 3);
    }

    strncat(body, "\r\n", 3);

    write_file_contents_to_body(body, res->path);
    //// printf("line274 %s\n", body);
    strncat(body, "\r\n\0", 4);

    printf("response size:- %ld\n", strlen(body));

    char *final_response_body = (char *)malloc(sizeof(char) * (strlen(body) + 1));

    printf("final body: %p\n", final_response_body);

    strncpy(final_response_body, body, (strlen(body) + 1));

    // printf("-------------------------response -------------------------------\n%s", body);
    // strncat
    free(body);

    return final_response_body;
}

void accept_incoming_request(int socket, struct sockaddr_in *address, socklen_t len)
{

    int accept_fd;

    while ((accept_fd = accept(socket, (struct sockaddr *)address, &len)) > 0)
    {

        char *buffer = (char *)malloc(sizeof(char) * MAX_REQ_SIZE);

        int recvd_bytes = read(accept_fd, buffer, MAX_REQ_SIZE);

        // printf("%s\n%ld\n%d\n\n\n", buffer, strlen(buffer), recvd_bytes);

        buffer[recvd_bytes] = '\0';

        Request *req = parse_http_req(buffer, strlen(buffer));

        Response *res = handle_request(req);

        char *response = arrange_response(res);

        free(buffer);
        freeRequest(req);
        freeResponse(res);

        send(accept_fd, response, strlen(response), 0);

        free(response);

        close(accept_fd);
    }

    shutdown(socket, SHUT_RDWR);
}

int create_socket(struct sockaddr_in *address, socklen_t len)
{

    int fd, opt = 1;

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket creation failed\n");
        return -1;
    }

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        return -1;
    }

    if (bind(fd, (struct sockaddr *)address, len) < 0)
    {
        perror("Socket bind failed\n");
        return -1;
    }

    return fd;
}

int main(int argc, char *argv[])
{

    short PORT = DEFAULT_PORT;

    char *PROTOCOL = DEFAULT_PROTOCOL;

    if (argc > 1)
    {
        PORT = (short)atoi(argv[1]);
    }

    if (argc > 2)
    {
        PROTOCOL = argv[2];
    }

    // printf("%d %s\n", PORT, PROTOCOL);

    struct sockaddr_in address = {
        sin_family : AF_INET,
        sin_addr : {
            s_addr : INADDR_ANY
        },
        sin_port : htons(PORT)
    };

    int socket;

    if ((socket = create_socket(&address, sizeof(address))) < 0)
    {
        exit(1);
    }

    if (listen(socket, MAX_REQ_QUEUE_SIZE) < 0)
    {
        perror("Listen failed\n");
        exit(1);
    }

    accept_incoming_request(socket, &address, sizeof(address));

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <unistd.h>
#include <asm-generic/socket.h>
#include <string.h>


#ifndef REQUEST_HEADER_DEFINE__
#define REQUEST_HEADER_DEFINE__
#include "./includes/request.h"
#endif // REQUEST_HEADER_DEFINE__


#ifndef RESPONSE_HEADER_DEFINE__
#define RESPONSE_HEADER_DEFINE__
#include "./includes/response.h"
#endif

#define DEFAULT_PORT 8080
#define MAX_REQ_QUEUE_SIZE 10
#define MAX_REQ_SIZE 1024
#define DEFAULT_PROTOCOL "http"


void accept_incoming_request(int socket, struct sockaddr_in *address, socklen_t len)
{

    int accept_fd;

    while ((accept_fd = accept(socket, (struct sockaddr *)address, &len)) > 0)
    {

        char buffer[MAX_REQ_SIZE] = {'\0'}; // (char *)malloc(sizeof(char) * MAX_REQ_SIZE);

        int recvd_bytes = read(accept_fd, buffer, MAX_REQ_SIZE);

        // printf("%s\n%ld\n%d\n\n\n", buffer, strlen(buffer), recvd_bytes);

        buffer[recvd_bytes] = '\0';

        Request req = parse_http_req(buffer, strlen(buffer));

        Response res = handle_response(&req);

        char *response = arrange_response(&res);

        // free(buffer);

        // buffer = NULL;

        freeRequest(&req);

        freeResponse(&res);

        printf("Sending this response back with len: %ld\n %s\n", strlen(response), response);

        send(accept_fd, response, strlen(response), 0);
        

        free(response);
        response = NULL;

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
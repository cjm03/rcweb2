/*
 *  main.c
 *  
 *  Start the server, load metadata, and set up sockets
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <stdbool.h>
#include <dirent.h>

#include "router.h"
#include "ht2.h"
#include "parse.h"

#define PORT 8080
#define BUFFER_SIZE 4096

static Table table = {0};

int main(void)
{
    /* Initialize and fill hash table */
    printf("Initializing hashtable\n");
    scanDir(&table, "media");
    printf("Indexed clips:\n");

    printf("\n\n\nStarting server...\n");

    /* Start server */
    int server_fd = socket(AF_INET, SOCK_STREAM, 0); // assigns server_fd with a file descriptor referring to the endpoint created by socket()
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr); // converts 127.0.0.1 into binary network address structure then copies it into addr.sin_addr 
    addr.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) != 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    /* Server started successfully */
    printf("Server running on port %d\n", PORT);

    /* Deal with client connections to server */
    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        char buffer[BUFFER_SIZE] = {0};

        read(client_fd, buffer, BUFFER_SIZE - 1);

        struct Request* req = parseRequest(buffer);
        if (req) printf("Testing Method: %d\n", req->method);
        //     printf("URI: %s\n", req->url);
        //     printf("Version: %s\n", req->version);
        //     puts("Headers:");
        //     struct Header* h;
        //     for (h = req->headers; h; h = h->next) {
        //         printf("%-32s: %s\n", h->name, h->value);
        //     }
        //     puts("msgbody:");
        //     puts(req->body);
        // }
        handleRequest(client_fd, req, &table);
        close(client_fd);
        freeRequest(req);

        // printf("Request:\n%s\n", buffer);
        // handleRequest(client_fd, buffer);

    }
    freeTable(&table);
    return 0;
}


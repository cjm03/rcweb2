/*
 *  router.c
 *  
 *  Route paths
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "router.h"
#include "video.h"
#include "hashtable.h"
#include "utils.h"
#include "parse.h"

void handleRequest(Table* t, int client_fd, struct Request* req)// void handleRequest(int client_fd, const char* request)
{
    const char* not_found = "HTTP/1.1 404 Not Found\r\n\r\nNot Found";
    if (req->method == GET) {
        const char* resource = req->url;
        printf("\nResource: %s\n\nLength: %lu\n", resource, strlen(resource));
        if (strncmp(resource, "/clip?id=", 9) == 0 && strlen(resource) > 16) {

            const char* idparam = strstr(resource, "id=");

            if (idparam) {

                char clip_id[256];
                char raw_id[256];
                sscanf(idparam + 3, "%255[^ \r\n]", raw_id);
                urldecode(clip_id, raw_id);
                printf("Serving video: %s\n", clip_id);
                serveVideo(t, client_fd, clip_id);
                return;
            }

        } else if (strncmp(resource, "/", 1) == 0 && strlen(resource) == 1) {

            serveFile(client_fd, "public/index.html");
            return;

        } else if (strncmp(resource, "/api/clips", 10) == 0) {

            serveFile(client_fd, "public/clips.json");
            return;

        } else {

            printf("%s %s\n", not_found, resource);
            write(client_fd, not_found, strlen(not_found));
            close(client_fd);
            return;

        }
    }
    write(client_fd, not_found, strlen(not_found));
    return;
}

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
#include "ht2.h"
#include "utils.h"
#include "parse.h"

typedef struct {
    char* json;
    size_t offset;
} JsonBuffer;

void appendClipJson(Clip* clip, void* ctx)
{
    JsonBuffer* buf = ctx;
    if (buf->offset > 1) {
        buf->json[buf->offset++] = ',';
    }
    int written = snprintf(buf->json + buf->offset, 8192 - buf->offset,
                           "{\"id\":\"%s\",\"filename\":\"%s\",\"size\":%zu}",
                           clip->name, clip->path, clip->filesize);
    buf->offset += written;
}

void handleRequest(int client_fd, struct Request* req, Table* table)// void handleRequest(int client_fd, const char* request)
{
    const char* not_found = "HTTP/1.1 404 Not Found\r\n\r\nNot Found";
    if (req->method == GET) {
        const char* resource = req->url;
        printf("\nResource: %s\n\nLength: %lu\n", resource, strlen(resource));
        if (strncmp(resource, "/clip?id=", 9) == 0 && strlen(resource) > 16) {
            const char* idparam = strstr(resource, "id=");
            // printf("STRSTR: %s\n", idparam);
            if (idparam) {
                char clip_id[256];
                char raw_id[256];
                sscanf(idparam + 3, "%255[^ \r\n]", raw_id);
                urldecode(clip_id, raw_id);
                serveVideo(client_fd, clip_id, table);
                // close(client_fd);
                return;
            }
        } else if (strncmp(resource, "/", 1) == 0 && strlen(resource) == 1) {
            serveFile(client_fd, "public/index.html");
            // close(client_fd);
            return;
        } else if (strncmp(resource, "/api/clips", 10) == 0 && strlen(resource) == 10) {
            char json[8192] = "[";
            JsonBuffer buf = { .json = json, .offset = 1 };

            iterateClips(table, appendClipJson, &buf);

            json[buf.offset++] = ']';
            json[buf.offset] = '\0';

            char header[256];
            snprintf(header, sizeof(header),
                     "HTTP/1.1 200 OK\r\n"
                     "Content-Type: application/json\r\n"
                     "Content-Length: %zu\r\n"
                     "Connection: close\r\n\r\n",
                     buf.offset);

            write(client_fd, header, strlen(header));
            write(client_fd, json, buf.offset);
            close(client_fd);
            return;
        } else {
            printf("%s %s\n", not_found, resource);
            write(client_fd, not_found, strlen(not_found));
            close(client_fd);
            return;
        }
    }
    // if (strncmp(request, "GET /clip?", 10) == 0) {
    //     const char* id_param = strstr(request, "id=");
    //     printf("\nSTRSTR: %s\n", id_param);
    //     if (id_param) {
    //
    //         // int start = 3;// int end = 11;// int length = end - start;// char* clip_id = malloc(length + 1);// strncpy(clip_id, id_param + start, length);// clip_id[length] = '\0';// printf("\nSTRNCPY: '%s'\n", clip_id);
    //
    //         char clip_id[256] = {0};
    //         char raw_id[256] = {0};
    //         sscanf(id_param + 3, "%255[^ \r\n]", raw_id);
    //         urldecode(clip_id, raw_id);
    //         serveVideo(client_fd, clip_id);
    //         // free(clip_id);
    //         return;
    //     }
    // }

    // if (strncmp(request, "GET / ", 6) == 0 || strncmp(request, "GET /HTTP", 9) == 0) {
    //     serveFile(client_fd, "public/index.html");
    //     return;
    // }

    // if (strncmp(request, "GET /api/clips", 14) == 0) {
    //     char json[8192] = "[";
    //     JsonBuffer buf = { .json = json, .offset = 1 };
    //
    //     iterateClips(appendClipJson, &buf);
    //
    //     json[buf.offset++] = ']';
    //     json[buf.offset] = '\0';
    //
    //     char header[256];
    //     snprintf(header, sizeof(header),
    //              "HTTP/1.1 200 OK\r\n"
    //              "Content-Type: application/json\r\n"
    //              "Content-Length: %zu\r\n"
    //              "Connection: close\r\n\r\n",
    //              buf.offset);
    //
    //     write(client_fd, header, strlen(header));
    //     write(client_fd, json, buf.offset);
    //     return;
    // }

    // const char* not_found = "HTTP/1.1 404 Not Found\r\n\r\nNot Found";
    write(client_fd, not_found, strlen(not_found));
    return;
}

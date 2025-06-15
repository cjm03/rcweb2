/*
 *  video.c
 *  
 *  Load video metadata and serve video files
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <stdbool.h>
#include <errno.h>

#include "video.h"
#include "ht2.h"

#define BUFFER_SIZE 4096

void serveFile(int client_fd, const char* filepath)
{
    int file_fd = open(filepath, O_RDONLY);
    if (file_fd < 0) {
        const char* not_found = "HTTP/1.1 404 Not Found\r\n\r\nFile Not Found";
        write(client_fd, not_found, strlen(not_found));
        return;
    }

    struct stat st;
    fstat(file_fd, &st);

    char header[BUFFER_SIZE];
    snprintf(header, sizeof(header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Length: %lld\r\n"
             "Content-Type: text/html\r\n"
             "Connection: close\r\n\r\n", (long long)st.st_size);

    write(client_fd, header, strlen(header));

    off_t offset = 0;
    off_t len = st.st_size;
    if (sendfile(file_fd, client_fd, offset, &len, NULL, 0) == -1) {
        if (errno == EPIPE || errno == ECONNRESET) {
            printf("sendfile ok");
        } else {
            printf("SFErrno %d :: %s :: filepath: %s\n", errno, strerror(errno), filepath);
            // perror("sendfile");
        }
    }
    close(file_fd);
    close(client_fd);
    return;
}

void serveVideo(int client_fd, const char* clip_id, Table* table)
{
    Clip* clip = getClip(table, clip_id);
    if (!clip) {
        const char* not_found = "HTTP/1.1 404 Not Found\r\n\r\nClip not Found";
        write(client_fd, not_found, strlen(not_found));
        return;
    }

    int file_fd = open(clip->path, O_RDONLY);
    if (file_fd < 0) {
        const char* error = "HTTP/1.1 500 Internal Server Error\r\n\r\nFailed to open file";
        write(client_fd, error, strlen(error));
        return;
    }

    char header[BUFFER_SIZE];
    snprintf(header, sizeof(header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Length: %zu\r\n"
             "Content-Type: video/mp4\r\n"
             "Connection: close\r\n\r\n", clip->filesize);
    write(client_fd, header, strlen(header));

    off_t offset = 0;
    off_t len = clip->filesize;
    if (sendfile(file_fd, client_fd, offset, &len, NULL, 0) == -1) {
        if (errno == EPIPE || errno == ECONNRESET) {
            printf("sendfile ok");
        } else {
            // perror("sendfile");
            printf("SVErrno %d :: %s :: clipid: %s\n", errno, strerror(errno), clip_id);
        }
    }
    close(file_fd);
    free(clip->name);
    free(clip->path);
    free(clip->next);
    free(clip);
    close(client_fd);
    return;
}


#ifndef VIDEO_H
#define VIDEO_H

#include "ht2.h"

#define BUFFER_SIZE 4096

void serveFile(int client_fd, const char* filepath);
void serveVideo(int client_fd, const char* clip_id, Table* table);
void serveClipPage(int client_fd, const char* clip_id);

#endif

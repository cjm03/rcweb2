#ifndef ROUTER_H
#define ROUTER_H

#include "parse.h"
#include "hashtable.h"


/* Function to determine appropriate way to respond to client request 
 * Current Possibilities:
 *      GET /           
 *      GET /clip?      
*/
void handleRequest(Table* t, int client_fd, struct Request* req);
// void handleRequest(int client_fd, const char* request);

#endif

#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include "general_utils.h"

int start_server(int server_fd, struct sockaddr_in address);
int accept_connection(int server_fd);

#endif // SERVER_UTILS_H

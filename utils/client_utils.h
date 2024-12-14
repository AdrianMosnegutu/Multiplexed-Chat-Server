#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H

#include "general_utils.h"

int connect_to_address(int client_fd, struct sockaddr_in address);

#endif // CLIENT_UTILS_H

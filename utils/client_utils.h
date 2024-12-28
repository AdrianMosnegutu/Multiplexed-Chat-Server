#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H

#include "general_utils.h"

/*
 *  @brief Connect a client socket to a given IPv4 address.
 *
 *  @param `client_fd` The client socket we want to connect.
 *  @param `address` The address we want to connect to.
 *  @return `0` if the client socket connected successfuly, `-1` otherwise
 */
int connect_to_address(int client_fd, struct sockaddr_in address);

#endif // CLIENT_UTILS_H

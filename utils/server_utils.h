#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include "general_utils.h"

/*
 *  @brief Start a server, given a socket and IPv4 address.
 *
 *  Bind the socket to the provided IPv4 address and listen for
 *  incoming connections.
 *
 *  @param `server_fd` The server socket through which we accept
 *  incoming connections.
 *  @param `address` The server's IPv4 address.
 *
 *  @return `0` if the server socket is valid and no errors were
 *  thrown, `-1` otherwise.
 */
int start_server(int server_fd, struct sockaddr_in address);

/*
 *  @brief Accept an incoming connection.
 *
 *  Accepts an incoming client connection through the provided server
 *  socket and prints the details of the connection to the console.
 *
 *  @param `server_fd` The server socket through which we accept
 *  incoming connections
 *
 *  @return The newly connected client's socket if no errors occur,
 *  `-1` otherwise.
 */
int accept_connection(int server_fd);

#endif // SERVER_UTILS_H

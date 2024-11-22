#pragma once

#include "general_utils.h"

/**
 * @brief Closes all client sockets and the server socket.
 *
 * This function iterates through all file descriptors up to the maximum file descriptor
 * and closes any that are set in the file descriptor set, excluding the standard input
 * and the server socket.
 *
 * @param server_socket The server socket to be closed.
 * @param fds Pointer to the set of file descriptors.
 * @param max_fd The maximum file descriptor value.
 */
void close_all_client_sockets(socket_t server_socket, fd_set *fds, socket_t max_fd);

/**
 * @brief Shuts down the server and prints an error message.
 *
 * This function closes the server socket and prints the provided error message
 * to standard error output. If the error message is empty, the program exits
 * with a success status.
 *
 * @param server_socket The socket descriptor of the server to be shut down.
 * @param error_message A string containing the error message to be printed.
 */
void shutdown_server(socket_t server_socket, const char *error_message);

/**
 * Binds a socket to a specified address.
 *
 * @param socket_fd The file descriptor of the socket to bind.
 * @param address A pointer to a struct sockaddr_in containing the address to bind the socket to.
 */
void bind_socket_to_address(socket_t socket_fd, struct sockaddr_in *address);

/**
 * @brief Listens for incoming connections on the specified socket.
 *
 * This function sets up the specified socket to listen for incoming
 * connection requests. The socket must already be bound to an address
 * and port before calling this function.
 *
 * @param socket_fd The file descriptor of the socket to listen on.
 * @param backlog The maximum length to which the queue of pending
 *                connections for socket_fd may grow.
 */
void listen_for_incoming_connections(socket_t socket_fd, uint backlog);

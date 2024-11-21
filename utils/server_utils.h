#pragma once

#include "socket_utils.h"

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
void close_all_client_sockets(socket_t server_socket, fd_set *fds, socket_t max_fd) {
    for (socket_t fd = 0; fd <= max_fd; ++fd) {
        // Skip the standard input and the server socket
        if (fd == STDIN_FILENO || fd == server_socket) {
            continue;
        }

        if (FD_ISSET(fd, fds)) {
            close(fd);
        }
    }
}

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
void shutdown_server(socket_t server_socket, const char *error_message) {
    shutdown(server_socket, SHUT_RDWR);

    if (strlen(error_message) > 0) {
        terminate_with_error(error_message);
    } else {
        exit(EXIT_SUCCESS);
    }
}

/**
 * Binds a socket to a specified address.
 *
 * @param socket_fd The file descriptor of the socket to bind.
 * @param address A pointer to a struct sockaddr_in containing the address to bind the socket to.
 */
void bind_socket_to_address(socket_t socket_fd, struct sockaddr_in *address) {
    if (bind(socket_fd, (struct sockaddr *)address, sizeof *address) < 0) {
        shutdown_server(socket_fd, "Bind error");
    }
}

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
void listen_for_incoming_connections(socket_t socket_fd, uint backlog) {
    if (listen(socket_fd, backlog) < 0) {
        shutdown_server(socket_fd, "Listen error");
    }

    printf("Listening for connections on %s:%d...\n", SERVER_IP, SERVER_PORT);
}

/**
 * @brief Handles console commands for the server.
 *
 * This function processes commands entered in the server console and performs
 * the appropriate actions based on the command.
 * If the command is "quit", the server is shut down, otherwise it is ignored.
 *
 * @param server_socket The socket descriptor for the server.
 * @param master_fds A pointer to the set of file descriptors used by the server.
 * @param max_fd The maximum file descriptor value.
 */
void handle_console_command(socket_t server_socket, fd_set *master_fds, socket_t max_fd);

/**
 * @brief Handles an incoming connection on the server socket.
 *
 * This function accepts a new connection on the given server socket,
 * adds the new socket to the master file descriptor set, and updates
 * the maximum file descriptor value if necessary.
 *
 * @param server_socket The server socket that is listening for incoming connections.
 * @param master_fds Pointer to the set of master file descriptors.
 * @param max_fd Pointer to the maximum file descriptor value.
 */
void handle_incoming_connection(socket_t server_socket, fd_set *master_fds, socket_t *max_fd);

/**
 * @brief Handles communication between the server and a connected client.
 *
 * This function is responsible for managing the data exchange between the server
 * and a specific client. It reads data from the client, processes it, and broadcasts
 * the message to all other connected clients.
 *
 * @param server_socket The socket descriptor for the server.
 * @param client_socket The socket descriptor for the connected client.
 */
void handle_communication_with_client(socket_t server_socket, socket_t client_socket, fd_set *master_fds, socket_t max_fd);

/**
 * @brief Broadcasts a message to all connected clients except the sender and the server.
 *
 * This function sends a message from the sending socket to all other sockets
 * in the master file descriptor set, excluding the server socket and the
 * sending socket itself.
 *
 * @param server_socket The socket descriptor for the server.
 * @param sending_socket The socket descriptor of the client sending the message.
 * @param master_fds Pointer to the set of master file descriptors.
 */
void broadcast_message(socket_t server_socket, socket_t sending_socket, fd_set *master_fds, socket_t max_fd, char *message);


#pragma once

#include "socket_utils.h"

/**
 * @brief Connects a socket to a specified address.
 *
 * This function attempts to connect the given socket file descriptor to the
 * provided address.
 *
 * @param socket_fd The file descriptor of the socket to be connected.
 * @param address A pointer to a struct sockaddr_in containing the address to connect to.
 */
void connect_socket_to_address(socket_t socket_fd, struct sockaddr_in *address) {
    if (connect(socket_fd, (struct sockaddr *)address, sizeof(*address)) < 0) {
        close(socket_fd);
        terminate_with_error("Connect error");
    }

    printf("Connected to server at %s:%d\n", SERVER_IP, SERVER_PORT);
}

/**
 * @brief Reads the username from the input and stores it in the provided buffer.
 *
 * This function prompts the user to enter their username and reads the input
 * into the provided `username` buffer. The buffer should be large enough to
 * hold the username and a null terminator.
 * If there is no input, the username will be set to "Anonymous".
 *
 * @param username A pointer to a character array where the username will be stored.
 */
void read_username(char *username);

/**
 * @brief Handles communication with the server for a given client.
 *
 * This function manages the interaction between the client and the server,
 * sending and receiving messages as necessary. It operates in a loop,
 * continuously handling communication until the connection is closed.
 *
 * @param client_socket The socket descriptor for the client connection.
 * @param username The username of the client, used for identification.
 */
void handle_communication_with_server(socket_t client_socket, const char *username);

#pragma once

#include "general_utils.h"

/**
 * @brief Connects a socket to a specified address.
 *
 * This function attempts to connect the given socket file descriptor to the
 * provided address.
 *
 * @param socket_fd The file descriptor of the socket to be connected.
 * @param address A pointer to a struct sockaddr_in containing the address to connect to.
 */
void connect_socket_to_address(socket_t socket_fd, struct sockaddr_in *address);

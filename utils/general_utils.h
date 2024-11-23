#pragma once

#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#define SERVER_IP "127.0.0.1"  // The server IP address
#define SERVER_PORT 9999       // The server port

#define MAX_USERNAME_LENGTH 32                                          // The maximum length of a username
#define MAX_MESSAGE_LENGTH 256                                          // The maximum length of a message
#define MAX_RESPONSE_LENGTH (MAX_USERNAME_LENGTH + MAX_MESSAGE_LENGTH)  // The maximum length of a response

/**
 * @typedef socket_t
 * @brief A type definition for a socket descriptor.
 *
 * This typedef defines `socket_t` as an alias for `int`, representing a socket
 * descriptor used in network communication.
 */
typedef int socket_t;

/**
 * @brief Returns the maximum of two integers.
 *
 * This function takes two integer arguments and returns the greater of the two.
 *
 * @param a The first integer.
 * @param b The second integer.
 * @return The greater of the two integers.
 */
int max(int a, int b);

/**
 * @brief Terminates the program and prints the provided message.
 *
 * This function prints the specified message to the standard error output
 * and then terminates the program with a failure status.
 *
 * @param message The message to be printed before termination.
 */
void terminate_with_error(const char *error_message);

/**
 * @brief Creates a TCP socket.
 *
 * This function initializes and returns a TCP socket.
 *
 * @return socket_t The created TCP socket.
 */
socket_t create_tcp_socket();

/**
 * @brief Initializes an IPv4 address structure.
 *
 * This function initializes a sockaddr_in structure with the specified
 * host and port. The host can be an IP address in dotted-decimal format
 * or a hostname that will be resolved to an IP address.
 *
 * @param host The hostname or IP address in dotted-decimal format.
 * @param port The port number.
 * @return A sockaddr_in structure initialized with the specified host and port.
 */
struct sockaddr_in initialize_ipv4_address(const char *host, uint port);

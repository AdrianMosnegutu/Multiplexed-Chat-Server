#pragma once

#define SERVER_IP "127.0.0.1"  // The server IP address
#define SERVER_PORT 9999       // The server port

/**
 * @typedef socket_t
 * @brief A type definition for a socket descriptor.
 *
 * This typedef defines `socket_t` as an alias for `int`, representing a socket
 * descriptor used in network communication.
 */
typedef int socket_t;

const uint MAX_USERNAME_LENGTH = 32;
const uint MAX_MESSAGE_LENGTH = 256;
const uint MAX_RESPONSE_LENGTH = MAX_USERNAME_LENGTH + MAX_MESSAGE_LENGTH;

/**
 * @brief Returns the maximum of two socket descriptors.
 *
 * This function compares two socket descriptors and returns the one with the higher value.
 *
 * @param a The first socket descriptor.
 * @param b The second socket descriptor.
 * @return The socket descriptor with the higher value.
 */
socket_t max_sock(socket_t a, socket_t b) { 
    return a > b ? a : b; 
}

/**
 * @brief Terminates the program and prints the provided message.
 *
 * This function prints the specified message to the standard error output
 * and then terminates the program with a failure status.
 *
 * @param message The message to be printed before termination.
 */
void terminate_with_error(const char *error_message) {
    perror(error_message);
    exit(EXIT_FAILURE);
}

/**
 * @brief Creates a TCP socket.
 *
 * This function initializes and returns a TCP socket.
 *
 * @return socket_t The created TCP socket.
 */
socket_t create_tcp_socket() {
    socket_t socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0) {
        terminate_with_error("Socket error");
    }

    return socket_fd;
}

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
struct sockaddr_in initialize_ipv4_address(const char *host, uint port) {
    struct sockaddr_in address;

    address.sin_family = AF_INET;    // IPv4 address family
    address.sin_port = htons(port);  // convert to network byte order

    if (strlen(host) > 0) {
        address.sin_addr.s_addr = inet_addr(host);  // the given host
    } else {
        address.sin_addr.s_addr = INADDR_ANY;  // any available
    }

    return address;
}

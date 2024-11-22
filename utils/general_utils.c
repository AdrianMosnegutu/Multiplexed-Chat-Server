#include "general_utils.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/socket.h>

socket_t max_sock(socket_t a, socket_t b) { 
    return a > b ? a : b;
}

void terminate_with_error(const char *error_message) {
    perror(error_message);
    exit(EXIT_FAILURE);
}

socket_t create_tcp_socket() {
    socket_t socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0) {
        terminate_with_error("Socket error");
    }

    return socket_fd;
}

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

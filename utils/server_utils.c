#include "server_utils.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

int start_server(int server_fd, struct sockaddr_in address) {
    // Check the server socket's validity
    if (server_fd < 0) {
        return -1;
    }

    // Bind the server socket to the provided IPv4 address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        return -1;
    }

    // Listen for incoming connections
    if (listen(server_fd, 10) < 0) {
        return -1;
    }

    printf("Listening on %s:%d...\n", IP_ADDRESS, ntohs(address.sin_port));
    return EXIT_SUCCESS;
}

int accept_connection(int server_fd) {
    int client_fd;
    struct sockaddr_in client_address;
    socklen_t client_size = sizeof(client_address);

    // Accept the incoming connection
    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_size)) < 0) {
        return -1;
    }

    const char *incoming_ip_address = inet_ntoa(client_address.sin_addr);
    int incoming_port = ntohs(client_address.sin_port);
    printf("Connection accepted from %s:%d...\n", incoming_ip_address, incoming_port);

    return client_fd;
}

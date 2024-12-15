#include "client_utils.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

int connect_to_address(int client_fd, struct sockaddr_in address) {
    // Check the client socket's validity
    if (client_fd < 0) {
        return -1;
    }

    // Connect the client socket to the provided IPv4 address
    if ((connect(client_fd, (struct sockaddr *)&address, sizeof(address))) < 0) {
        return -1;
    }

    const char *incoming_ip_address = inet_ntoa(address.sin_addr);
    int incoming_port = ntohs(address.sin_port);
    printf("Connected to %s:%d...\n", incoming_ip_address, incoming_port);

    return EXIT_SUCCESS;
}

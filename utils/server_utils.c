#include "server_utils.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

int start_server(int server_fd, struct sockaddr_in address) {
    if (server_fd < 0) {
        return EXIT_FAILURE;
    }

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        return EXIT_FAILURE;
    }

    if (listen(server_fd, 10) < 0) {
        return EXIT_FAILURE;
    }

    printf("Listening on %s:%d...\n", IP, ntohs(address.sin_port));
    return EXIT_SUCCESS;
}

int accept_connection(int server_fd) {
    int client_fd;
    struct sockaddr_in client_address;
    socklen_t client_size = sizeof(client_address);

    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_size)) < 0) {
        shutdown(server_fd, SHUT_RDWR);
        perror("Accept error");
        exit(EXIT_FAILURE);
    }

    printf("Connection accepted from %s:%d...\n", inet_ntoa(client_address.sin_addr),
           ntohs(client_address.sin_port));
    return client_fd;
}

#include "server_utils.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

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

void shutdown_server(socket_t server_socket, const char *error_message) {
    shutdown(server_socket, SHUT_RDWR);

    if (strlen(error_message) > 0) {
        terminate_with_error(error_message);
    } else {
        exit(EXIT_SUCCESS);
    }
}

void bind_socket_to_address(socket_t socket_fd, struct sockaddr_in *address) {
    if (bind(socket_fd, (struct sockaddr *)address, sizeof *address) < 0) {
        shutdown_server(socket_fd, "Bind error");
    }
}

void listen_for_incoming_connections(socket_t socket_fd, uint backlog) {
    if (listen(socket_fd, backlog) < 0) {
        shutdown_server(socket_fd, "Listen error");
    }

    printf("Listening for connections on %s:%d...\n", SERVER_IP, SERVER_PORT);
}

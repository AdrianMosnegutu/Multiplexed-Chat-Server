#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils/server_utils.h"

void handle_console_command(socket_t server_socket, fd_set *master_fds, socket_t max_fd);
void handle_incoming_connection(socket_t server_socket, fd_set *master_fds, socket_t *max_fd);
void handle_communication_with_client(socket_t server_socket, socket_t client_socket, fd_set *master_fds, socket_t max_fd);
void broadcast_message(socket_t server_socket, socket_t sending_socket, fd_set *master_fds, socket_t max_fd, char *message);

int main(int argc, char *argv[]) {
    socket_t server_socket = create_tcp_socket();
    struct sockaddr_in address = initialize_ipv4_address("", SERVER_PORT);

    bind_socket_to_address(server_socket, &address);
    listen_for_incoming_connections(server_socket, 10);

    // Initialize the file descriptor set
    fd_set master_fds;
    FD_ZERO(&master_fds);
    FD_SET(server_socket, &master_fds);  // Listen for incoming connections
    FD_SET(STDIN_FILENO, &master_fds);   // Listen for console input

    // Determine the maximum file descriptor value
    socket_t max_fd = max_sock(server_socket, STDIN_FILENO);

    while (true) {
        fd_set copy_fds = master_fds;

        // Wait for activity on the file descriptors
        if (select(max_fd + 1, &copy_fds, NULL, NULL, NULL) < 0) {
            shutdown_server(server_socket, "Select error");
        }

        for (socket_t fd = 0; fd <= max_fd; ++fd) {
            if (FD_ISSET(fd, &copy_fds)) {
                if (fd == STDIN_FILENO) {
                    handle_console_command(server_socket, &master_fds, max_fd);
                } else if (fd == server_socket) {
                    handle_incoming_connection(server_socket, &master_fds, &max_fd);
                } else {
                    handle_communication_with_client(server_socket, fd, &master_fds, max_fd);
                }
            }
        }
    }
}

void handle_incoming_connection(socket_t server_socket, fd_set *master_fds, socket_t *max_fd) {
    struct sockaddr_in client_address;
    socklen_t client_size = sizeof client_address;

    char username[MAX_USERNAME_LENGTH] = {0};
    char response[MAX_RESPONSE_LENGTH] = {0};

    // Accept the incoming connection
    socket_t client_socket =
        accept(server_socket, (struct sockaddr *)&client_address, &client_size);
    if (client_socket < 0) {
        printf("A client tried to connect but failed...\n");
        return;
    }

    // Receive the username from the client
    if (recv(client_socket, username, MAX_USERNAME_LENGTH, 0) <= 0) {
        close_all_client_sockets(server_socket, master_fds, *max_fd);
        shutdown_server(server_socket, "Recv error");
    }

    // Construct the response message
    snprintf(response, MAX_RESPONSE_LENGTH, "%s joined the group chat!", username);

    // Log and broadcast the new connection
    printf("%s connected from %s:%d\n", username, inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
    broadcast_message(server_socket, client_socket, master_fds, *max_fd, response);

    // Add the new client socket to the master set
    FD_SET(client_socket, master_fds);
    *max_fd = max_sock(*max_fd, client_socket);
}

void handle_communication_with_client(socket_t server_socket, socket_t client_socket, fd_set *master_fds, socket_t max_fd) {
    char response[MAX_RESPONSE_LENGTH] = {0};

    // Receive the message from the client
    if (recv(client_socket, response, sizeof response, 0) <= 0) {
        FD_CLR(client_socket, master_fds);
        strcpy(response, "A client left the group chat!");
    }

    // Log and broadcast the message
    printf("%s\n", response);
    broadcast_message(server_socket, client_socket, master_fds, max_fd, response);
}

void broadcast_message(socket_t server_socket, socket_t sending_socket, fd_set *master_fds, socket_t max_fd, char *message) {
    for (socket_t fd = 0; fd <= max_fd; ++fd) {
        // Skip the server socket, the sending socket, and the standard input
        if (fd == STDIN_FILENO || fd == server_socket || fd == sending_socket) {
            continue;
        }

        if (FD_ISSET(fd, master_fds)) {
            if (send(fd, message, strlen(message), 0) <= 0) {
                close_all_client_sockets(server_socket, master_fds, max_fd);
                shutdown_server(server_socket, "Broadcast error");
            }
        }
    }
}

void handle_console_command(socket_t server_socket, fd_set *master_fds, socket_t max_fd) {
    char command[16] = {0};
    fgets(command, sizeof command, stdin);
    command[strlen(command) - 1] = '\0';

    if (strcmp(command, "quit") == 0) {
        printf("Shutting down the server...\n");
        close_all_client_sockets(server_socket, master_fds, max_fd);
        shutdown_server(server_socket, "");
    } else {
        printf("Unknown command: %s\n", command);
    }
}

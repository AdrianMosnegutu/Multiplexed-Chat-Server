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

static void close_all_client_sockets();
static void handle_console_command();
static void handle_incoming_connection();
static void handle_communication_with_client(socket_t client_socket);
static void broadcast_message(socket_t sending_socket, char *message);

static socket_t server_socket;
static fd_set read_fds;
static int max_fd;
static char *usernames[RLIMIT_NOFILE];

int main(int argc, char *argv[]) {
    server_socket = create_tcp_socket();
    struct sockaddr_in address = initialize_ipv4_address("", SERVER_PORT);

    bind_socket_to_address(server_socket, &address);
    listen_for_incoming_connections(server_socket, 5);

    // Initialize the file descriptor set
    FD_ZERO(&read_fds);
    FD_SET(server_socket, &read_fds);  // Listen for incoming connections
    FD_SET(STDIN_FILENO, &read_fds);   // Listen for console input

    // Determine the maximum file descriptor value
    max_fd = max(server_socket, STDIN_FILENO);

    while (true) {
        fd_set copy_fds = read_fds;

        // Wait for activity on the file descriptors
        if (select(max_fd + 1, &copy_fds, NULL, NULL, NULL) < 0) {
            shutdown_server(server_socket, "Select error");
        }

        for (int fd = 0; fd <= max_fd; ++fd) {
            if (FD_ISSET(fd, &copy_fds)) {
                if (fd == STDIN_FILENO) {
                    handle_console_command();
                } else if (fd == server_socket) {
                    handle_incoming_connection();
                } else {
                    handle_communication_with_client(fd);
                }
            }
        }
    }
}

static void handle_incoming_connection() {
    struct sockaddr_in client_address;
    socklen_t client_size = sizeof client_address;

    // Accept the incoming connection
    socket_t client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_size);
    if (client_socket < 0) {
        printf("A client tried to connect but failed...\n");
        return;
    }

    // Receive the username from the client
    usernames[client_socket] = malloc(MAX_USERNAME_LENGTH);
    if (recv(client_socket, usernames[client_socket], MAX_USERNAME_LENGTH, 0) <= 0) {
        close_all_client_sockets();
        shutdown_server(server_socket, "Recv error");
    }

    // Construct the response message
    char response[MAX_RESPONSE_LENGTH] = {0};
    snprintf(response, MAX_RESPONSE_LENGTH, "%s joined the group chat!", usernames[client_socket]);

    // Log and broadcast the new connection
    printf("%s connected from %s:%d\n", usernames[client_socket], inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
    broadcast_message(client_socket, response);

    // Add the new client socket to the master set
    FD_SET(client_socket, &read_fds);
    max_fd = max(max_fd, client_socket);
}

static void handle_communication_with_client(socket_t client_socket) {
    char message[MAX_MESSAGE_LENGTH] = {0};
    char response[MAX_RESPONSE_LENGTH] = {0};

    // If the client disconnected, broadcast it to all clients, otherwise receive their response
    if (recv(client_socket, message, MAX_MESSAGE_LENGTH, 0) <= 0) {
        snprintf(response, MAX_RESPONSE_LENGTH, "%s left the group chat!", usernames[client_socket]);
        FD_CLR(client_socket, &read_fds);
        free(usernames[client_socket]);
    } else {
        snprintf(response, MAX_RESPONSE_LENGTH, "%s: %s", usernames[client_socket], message);
    }

    // Log and broadcast the message
    printf("%s\n", response);
    broadcast_message(client_socket, response);
}

static void broadcast_message(socket_t sending_socket, char *message) {
    for (int fd = 0; fd <= max_fd; ++fd) {
        // Skip the server socket, the sending socket, and the standard input
        if (fd == STDIN_FILENO || fd == server_socket || fd == sending_socket) {
            continue;
        }

        if (FD_ISSET(fd, &read_fds)) {
            if (send(fd, message, strlen(message), 0) <= 0) {
                close_all_client_sockets();
                shutdown_server(server_socket, "Broadcast error");
            }
        }
    }
}

static void handle_console_command() {
    char command[16] = {0};

    if (fgets(command, sizeof command, stdin) == NULL) {
        return;
    }
    command[strcspn(command, "\n")] = '\0';

    if (strcmp(command, "quit") == 0) {
        printf("Shutting down the server...\n");
        close_all_client_sockets();
        shutdown_server(server_socket, "");
    } else {
        printf("Unknown command: %s\n", command);
    }
}

static void close_all_client_sockets() {
    for (int fd = 0; fd <= max_fd; ++fd) {
        // Skip the standard input and the server socket
        if (fd == STDIN_FILENO || fd == server_socket) {
            continue;
        }

        if (FD_ISSET(fd, &read_fds)) {
            free(usernames[fd]);
            close(fd);
        }
    }
}

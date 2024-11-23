#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils/client_utils.h"

static void read_username();
static void handle_communication_with_server();

static socket_t client_socket;
static char username[MAX_USERNAME_LENGTH];

int main(int argc, char *argv[]) {
    client_socket = create_tcp_socket();
    struct sockaddr_in server_address = initialize_ipv4_address(SERVER_IP, SERVER_PORT);

    read_username();

    connect_socket_to_address(client_socket, &server_address);
    handle_communication_with_server();

    return 0;
}

static void handle_communication_with_server() {
    // Send the username to the server
    if (send(client_socket, username, strlen(username), 0) <= 0) {
        close(client_socket);
        terminate_with_error("Send error");
    }

    // Initialize the file descriptor set
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(client_socket, &read_fds);  // Listen for messages from the server
    FD_SET(STDIN_FILENO, &read_fds);   // Listen for input from the console

    // Determine the maximum file descriptor value
    socket_t max_fd = max(client_socket, STDIN_FILENO);

    while (true) {
        fd_set copy_fds = read_fds;

        // Wait for activity on the file descriptors
        if (select(max_fd + 1, &copy_fds, NULL, NULL, NULL) < 0) {
            close(client_socket);
            terminate_with_error("Select error");
        }

        // Handle input from the console
        if (FD_ISSET(STDIN_FILENO, &copy_fds)) {
            char message[MAX_MESSAGE_LENGTH] = {0};

            // Read the message from the console and remove the newline character
            if (fgets(message, MAX_MESSAGE_LENGTH, stdin) == NULL) {
                printf("Error reading from stdin...\n");
                continue;
            }
            message[strcspn(message, "\n")] = '\0';

            // Skip empty messages
            if (strlen(message) == 0) {
                continue;
            }

            // Send the message to the server
            if (send(client_socket, message, strlen(message), 0) <= 0) {
                close(client_socket);
                terminate_with_error("Send error");
            }
        }

        // Handle messages from the server
        if (FD_ISSET(client_socket, &copy_fds)) {
            char response[MAX_RESPONSE_LENGTH] = {0};

            // Receive the message from the server
            if (recv(client_socket, response, MAX_RESPONSE_LENGTH, 0) <= 0) {
                printf("Server closed the connection...\n");
                break;
            }

            printf("%s\n", response);
        }
    }

    // Clean up by closing the socket
    close(client_socket);
}

static void read_username() {
    printf("Enter username: ");

    if (fgets(username, MAX_USERNAME_LENGTH, stdin) == NULL) {
        printf("Error reading the username. Joining as Anonymous...\n");
        strcpy(username, "Anonymous");
        return;
    }
    username[strcspn(username, "\n")] = '\0';

    // Set a default username if none is provided
    if (strlen(username) == 0) {
        printf("No username provided. Joining as Anonymous...\n");
        strcpy(username, "Anonymous");
    }
}

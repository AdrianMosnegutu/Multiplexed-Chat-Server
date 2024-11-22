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

static void read_username(char *username);
static void handle_communication_with_server(socket_t client_socket, const char *username);

int main(int argc, char *argv[]) {
    socket_t client_socket = create_tcp_socket();
    struct sockaddr_in server_address = initialize_ipv4_address(SERVER_IP, SERVER_PORT);

    char username[MAX_USERNAME_LENGTH];
    read_username(username);

    connect_socket_to_address(client_socket, &server_address);
    handle_communication_with_server(client_socket, username);

    return 0;
}

static void handle_communication_with_server(socket_t client_socket, const char *username) {
    // Send the username to the server
    if (send(client_socket, username, strlen(username), 0) <= 0) {
        close(client_socket);
        terminate_with_error("Send error");
    }

    // Initialize the file descriptor set
    fd_set master_fds;
    FD_ZERO(&master_fds);
    FD_SET(client_socket, &master_fds);  // Listen for messages from the server
    FD_SET(STDIN_FILENO, &master_fds);   // Listen for input from the console

    // Determine the maximum file descriptor value
    socket_t max_fd = max_sock(client_socket, STDIN_FILENO);

    while (true) {
        fd_set copy_fds = master_fds;

        // Wait for activity on the file descriptors
        if (select(max_fd + 1, &copy_fds, NULL, NULL, NULL) < 0) {
            close(client_socket);
            terminate_with_error("Select error");
        }

        // Handle input from the console
        if (FD_ISSET(STDIN_FILENO, &copy_fds)) {
            char message[MAX_MESSAGE_LENGTH] = {0};
            char response[MAX_RESPONSE_LENGTH] = {0};

            // Read the message from the console and remove the newline character
            if (fgets(message, MAX_MESSAGE_LENGTH, stdin) == NULL) {
                printf("Error reading from stdin...\n");
                continue;
            }
            message[strlen(message) - 1] = '\0';

            // Skip empty messages
            if (strlen(message) == 0) {
                continue;
            }

            // Construct the new message
            snprintf(response, MAX_RESPONSE_LENGTH, "%s: %s", username, message);

            // Send the message to the server
            if (send(client_socket, response, strlen(response), 0) <= 0) {
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

static void read_username(char *username) {
    printf("Enter username: ");

    if (fgets(username, MAX_USERNAME_LENGTH, stdin) == NULL) {
        printf("Error reading the username. Using default username Anonymous...\n");
        strcpy(username, "Anonymous");
        return;
    }
    username[strcspn(username, "\n")] = '\0';

    // Set a default username if none is provided
    if (strlen(username) == 0) {
        strcpy(username, "Anonymous");
    }
}

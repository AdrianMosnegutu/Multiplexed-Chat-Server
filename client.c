#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils/client_utils.h"

void read_username();
void handle_communication();
void terminate_client(const char *error_message);

int client_fd;

char *username;
char *message;
char *response;

int main(int argc, char *argv[]) {
    username = (char *)malloc(USERNAME_LEN);
    message = (char *)malloc(MESSAGE_LEN);
    response = (char *)malloc(RESPONSE_LEN);

    read_username();

    if (connect_to_address(client_fd = create_tcp_socket(), create_ipv4_address(IP, PORT)) < 0) {
        terminate_client("Error connecting to the server");
    }

    handle_communication();
    terminate_client(NULL);
}

void handle_communication() {
    if (send(client_fd, username, strlen(username), 0) <= 0) {
        terminate_client("Send error");
    }

    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(client_fd, &read_fds);
    FD_SET(STDIN_FILENO, &read_fds);

    int max_fd = max(client_fd, STDIN_FILENO);

    while (1) {
        fd_set copy_fds = read_fds;

        if (select(max_fd + 1, &copy_fds, NULL, NULL, NULL) < 0) {
            terminate_client("Select error");
        }

        if (FD_ISSET(STDIN_FILENO, &copy_fds)) {
            memset(message, 0, MESSAGE_LEN);

            if (fgets(message, MESSAGE_LEN, stdin) == NULL) {
                printf("Error reading from stdin...\n");
                continue;
            }
            message[strcspn(message, "\n")] = '\0';

            if (strlen(message) > 0) {
                if (send(client_fd, message, strlen(message), 0) <= 0) {
                    terminate_client("Send error");
                }
            }
        }

        if (FD_ISSET(client_fd, &copy_fds)) {
            memset(response, 0, RESPONSE_LEN);

            if (recv(client_fd, response, RESPONSE_LEN, 0) <= 0) {
                printf("Server closed the connection...\n");
                break;
            }

            printf("%s\n", response);
        }
    }
}

void read_username() {
    printf("Enter username: ");

    if (fgets(username, USERNAME_LEN, stdin) == NULL) {
        printf("Error reading the username. Joining as Anonymous...\n");
        strcpy(username, "Anonymous");
        return;
    }
    username[strcspn(username, "\n")] = '\0';

    if (strlen(username) == 0) {
        printf("No username provided. Joining as Anonymous...\n");
        strcpy(username, "Anonymous");
    }
}

void terminate_client(const char *error_message) {
    close(client_fd);

    free(username);
    free(message);
    free(response);

    if (error_message) {
        perror(error_message);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

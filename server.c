#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils/server_utils.h"

void handle_console_command();
void handle_connection();
void handle_client(int client_fd);
void broadcast_message(int sending_fd, char *message);
void terminate_server(const char *error_message);

int server_fd;
fd_set read_fds;
int max_fd;

char **usernames;
char *message;
char *response;

int main(int argc, char *argv[]) {
    usernames = (char **)calloc(RLIMIT_NOFILE, sizeof(char *));
    message = (char *)malloc(MESSAGE_LEN);
    response = (char *)malloc(RESPONSE_LEN);

    if (start_server(server_fd = create_tcp_socket(), create_ipv4_address(NULL, PORT)) < 0) {
        terminate_server("Error starting the server");
    }

    FD_ZERO(&read_fds);
    FD_SET(server_fd, &read_fds);
    FD_SET(STDIN_FILENO, &read_fds);

    max_fd = max(server_fd, STDIN_FILENO);

    while (1) {
        fd_set copy_fds = read_fds;

        if (select(max_fd + 1, &copy_fds, NULL, NULL, NULL) < 0) {
            terminate_server("Select error");
        }

        for (int client_fd = 0; client_fd <= max_fd; ++client_fd) {
            if (!FD_ISSET(client_fd, &copy_fds)) {
                continue;
            }

            if (client_fd == STDIN_FILENO) {
                handle_console_command();
            } else if (client_fd == server_fd) {
                handle_connection();
            } else {
                handle_client(client_fd);
            }
        }
    }

    terminate_server(NULL);
}

void handle_connection() {
    int client_fd = accept_connection(server_fd);
    if (client_fd < 0) {
        printf("Client tried to connect but failed\n");
        return;
    }

    FD_SET(client_fd, &read_fds);
    max_fd = max(max_fd, client_fd);

    usernames[client_fd] = (char *)malloc(USERNAME_LEN);
    if (recv(client_fd, usernames[client_fd], USERNAME_LEN, 0) <= 0) {
        terminate_server("Recv error");
    }

    memset(response, 0, RESPONSE_LEN);
    snprintf(response, RESPONSE_LEN, "%s joined the group chat!", usernames[client_fd]);

    printf("%s joined the group chat!\n", usernames[client_fd]);
    broadcast_message(client_fd, response);
}

void handle_client(int client_fd) {
    memset(message, 0, MESSAGE_LEN);
    memset(response, 0, RESPONSE_LEN);

    if (recv(client_fd, message, MESSAGE_LEN, 0) <= 0) {
        snprintf(response, RESPONSE_LEN, "%s left the group chat!", usernames[client_fd]);
        FD_CLR(client_fd, &read_fds);
        free(usernames[client_fd]);
    } else {
        snprintf(response, RESPONSE_LEN, "%s: %s", usernames[client_fd], message);
    }

    printf("%s\n", response);
    broadcast_message(client_fd, response);
}

void broadcast_message(int sending_fd, char *message) {
    for (int client_fd = 0; client_fd <= max_fd; ++client_fd) {
        if (client_fd == STDIN_FILENO || client_fd == server_fd || client_fd == sending_fd) {
            continue;
        }

        if (FD_ISSET(client_fd, &read_fds)) {
            if (send(client_fd, message, strlen(message), 0) <= 0) {
                terminate_server("Broadcast error");
            }
        }
    }
}

void handle_console_command() {
    char command[16] = {0};

    if (fgets(command, sizeof command, stdin) == NULL) {
        printf("Error reading from stdin...\n");
        return;
    }
    command[strcspn(command, "\n")] = '\0';

    if (strcmp(command, "quit") == 0) {
        printf("Shutting down the server...\n");
        terminate_server(NULL);
    } else {
        printf("Unknown command: %s\n", command);
    }
}

void terminate_server(const char *error_message) {
    for (int client_fd = 0; client_fd <= max_fd; ++client_fd) {
        if (client_fd == STDIN_FILENO || client_fd == server_fd) {
            continue;
        }

        if (FD_ISSET(client_fd, &read_fds)) {
            free(usernames[client_fd]);
            close(client_fd);
        }
    }

    free(usernames);
    free(message);
    free(response);

    if (server_fd >= 0) {
        shutdown(server_fd, SHUT_RDWR);
    }

    if (error_message) {
        perror(error_message);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

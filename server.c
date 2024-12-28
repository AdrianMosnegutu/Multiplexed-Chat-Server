#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils/server_utils.h"

void handle_communication();
void handle_console_command();
void handle_incoming_connection();
void handle_client(int client_fd);
void broadcast_message(int sending_fd, char *message);
void terminate_server(const char *error_message);

int server_fd;
fd_set read_fds;
int max_fd;

char **usernames;
char *message;
char *response;

int main(int argc, char *argv[])
{
    usernames = (char **)calloc(RLIMIT_NOFILE, sizeof(char *));
    message = (char *)malloc(MESSAGE_LEN);
    response = (char *)malloc(RESPONSE_LEN);

    if (start_server(server_fd = create_tcp_socket(), create_ipv4_address(NULL, PORT)) < 0)
    {
        terminate_server("Could not start server");
    }

    handle_communication();
    terminate_server(NULL);
}

void handle_communication()
{
    // Setup the file descriptor set
    FD_ZERO(&read_fds);
    FD_SET(server_fd, &read_fds);
    FD_SET(STDIN_FILENO, &read_fds);

    max_fd = max(server_fd, STDIN_FILENO);

    while (1)
    {
        fd_set copy_fds = read_fds;

        // Put into `copy_fds` the set of file descriptors which are ready for reading
        if (select(max_fd + 1, &copy_fds, NULL, NULL, NULL) < 0)
        {
            terminate_server("Select error");
        }

        // Iterate through all file descriptors
        for (int client_fd = 0; client_fd <= max_fd; ++client_fd)
        {
            // Skip if the current file descriptor is not yet ready for reading
            if (!FD_ISSET(client_fd, &copy_fds))
            {
                continue;
            }

            if (client_fd == STDIN_FILENO)
            {
                // If there is something to be read from the console
                handle_console_command();
            }
            else if (client_fd == server_fd)
            {
                // If there is an incoming connection
                handle_incoming_connection();
            }
            else
            {
                // If a client sent a message to the server
                handle_client(client_fd);
            }
        }
    }
}

void handle_incoming_connection()
{
    // Accept the incoming connection
    int client_fd;
    if ((client_fd = accept_connection(server_fd)) < 0)
    {
        printf("Client tried to connect but failed\n");
        return;
    }

    // Update the file descriptor set and current file descriptor max
    FD_SET(client_fd, &read_fds);
    max_fd = max(max_fd, client_fd);

    // Receive the username from the client
    usernames[client_fd] = (char *)malloc(USERNAME_LEN);
    if (recv(client_fd, usernames[client_fd], USERNAME_LEN, 0) <= 0)
    {
        terminate_server("Recv error");
    }

    // Create an appropriate response
    memset(response, 0, RESPONSE_LEN);
    snprintf(response, RESPONSE_LEN, "%s joined the group chat!", usernames[client_fd]);

    // Broadcast the arrival of a new client in the group chat
    printf("%s\n", response);
    broadcast_message(client_fd, response);
}

void handle_client(int client_fd)
{
    memset(message, 0, MESSAGE_LEN);
    memset(response, 0, RESPONSE_LEN);

    // Receive the response from the client
    if (recv(client_fd, message, MESSAGE_LEN, 0) <= 0)
    {
        // If the client disconnected, set the response to announce that they left the group chat
        snprintf(response, RESPONSE_LEN, "%s left the group chat!", usernames[client_fd]);
        FD_CLR(client_fd, &read_fds);
        free(usernames[client_fd]);
    }
    else
    {
        snprintf(response, RESPONSE_LEN, "%s: %s", usernames[client_fd], message);
    }

    // Broadcast either the message received from the client or the announcement that they left
    printf("%s\n", response);
    broadcast_message(client_fd, response);
}

void broadcast_message(int sending_fd, char *message)
{
    for (int client_fd = 0; client_fd <= max_fd; ++client_fd)
    {
        // Skip the standard input, server_fd and sending_fd
        if (client_fd == STDIN_FILENO || client_fd == server_fd || client_fd == sending_fd)
        {
            continue;
        }

        // Send the message to all other clients
        if (FD_ISSET(client_fd, &read_fds))
        {
            if (send(client_fd, message, strlen(message), 0) <= 0)
            {
                terminate_server("Broadcast error");
            }
        }
    }
}

void handle_console_command()
{
    char command[16] = {0};

    // Read the command from the console
    if (fgets(command, sizeof command, stdin) == NULL)
    {
        printf("Error reading from stdin...\n");
        return;
    }
    command[strcspn(command, "\n")] = '\0';

    if (strcmp(command, "quit") == 0)
    {
        printf("Shutting down the server...\n");
        terminate_server(NULL);
    }
    else
    {
        printf("Unknown command: %s\n", command);
    }
}

void terminate_server(const char *error_message)
{
    for (int client_fd = 0; client_fd <= max_fd; ++client_fd)
    {
        // The standard input should be left open, and the server should be closed separately
        if (client_fd == STDIN_FILENO || client_fd == server_fd)
        {
            continue;
        }

        // Close all client connections and deallocate their usernames
        if (FD_ISSET(client_fd, &read_fds))
        {
            free(usernames[client_fd]);
            close(client_fd);
        }
    }

    free(usernames);
    free(message);
    free(response);

    shutdown(server_fd, SHUT_RDWR);

    if (error_message)
    {
        perror(error_message);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

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
void handle_send_message();
void handle_receive_message();
void terminate_client(const char *error_message);

int client_fd;

char *username;
char *message;
char *response;

int main(int argc, char *argv[])
{
    username = (char *)malloc(USERNAME_LEN);
    message = (char *)malloc(MESSAGE_LEN);
    response = (char *)malloc(RESPONSE_LEN);

    read_username();

    if (connect_to_address(client_fd = create_tcp_socket(), create_ipv4_address(IP_ADDRESS, PORT)) < 0)
    {
        terminate_client("Server is closed");
    }

    handle_communication();
    terminate_client(NULL);
}

void handle_communication()
{
    // Send the username to the server
    if (send(client_fd, username, strlen(username), 0) <= 0)
    {
        terminate_client("Send error");
    }

    // Setup the file descriptor set
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(client_fd, &read_fds);
    FD_SET(STDIN_FILENO, &read_fds);

    int max_fd = max(client_fd, STDIN_FILENO);

    while (1)
    {
        fd_set copy_fds = read_fds;

        // Put into `copy_fds` the set of file descriptors which are ready for reading
        if (select(max_fd + 1, &copy_fds, NULL, NULL, NULL) < 0)
        {
            terminate_client("Select error");
        }

        // If there is something to be read from the console
        if (FD_ISSET(STDIN_FILENO, &copy_fds))
        {
            handle_send_message();
        }

        // If there is something to be received from the server
        if (FD_ISSET(client_fd, &copy_fds))
        {
            handle_receive_message();
        }
    }
}

void handle_send_message()
{
    memset(message, 0, MESSAGE_LEN);

    // Read the message from the console
    if (fgets(message, MESSAGE_LEN, stdin) == NULL)
    {
        printf("Error reading from stdin...\n");
        return;
    }
    message[strcspn(message, "\n")] = '\0';

    // Send the message to the server, as long as it is not an empty string
    if (strlen(message) > 0)
    {
        if (send(client_fd, message, strlen(message), 0) <= 0)
        {
            terminate_client("Send error");
        }
    }
}

void handle_receive_message()
{
    memset(response, 0, RESPONSE_LEN);

    // Receive the message from the server
    if (recv(client_fd, response, RESPONSE_LEN, 0) <= 0)
    {
        printf("Server closed the connection...\n");
        terminate_client(NULL);
    }

    // Display it to the client
    printf("%s\n", response);
}

void read_username()
{
    printf("Enter username: ");

    // If reading the username from the console fails, it defaults to 'Anonymous'
    if (fgets(username, USERNAME_LEN, stdin) == NULL)
    {
        printf("Error reading the username. Joining as Anonymous...\n");
        strcpy(username, "Anonymous");
        return;
    }
    username[strcspn(username, "\n")] = '\0';

    // If no username was provided, it defaults to 'Anonymous'
    if (strlen(username) == 0)
    {
        printf("No username provided. Joining as Anonymous...\n");
        strcpy(username, "Anonymous");
    }
}

void terminate_client(const char *error_message)
{
    close(client_fd);

    free(username);
    free(message);
    free(response);

    if (error_message != NULL)
    {
        printf("%s\n", error_message);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

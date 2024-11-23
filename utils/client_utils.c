#include "client_utils.h"

#include <arpa/inet.h>
#include <unistd.h>

void connect_socket_to_address(socket_t socket_fd, struct sockaddr_in *address) {
    if (connect(socket_fd, (struct sockaddr *)address, sizeof(*address)) < 0) {
        close(socket_fd);
        terminate_with_error("Connect error");
    }

    printf("Connected to %s:%d\n", SERVER_IP, SERVER_PORT);
}

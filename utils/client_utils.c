#include "client_utils.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

int connect_to_address(int client_fd, struct sockaddr_in address) {
    if (client_fd < 0) {
        return EXIT_FAILURE;
    }

    if (connect(client_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        return EXIT_FAILURE;
    }

    printf("Connected to %s:%d...\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
    return EXIT_SUCCESS;
}
